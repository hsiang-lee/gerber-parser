#include "primitive_item.h"
#include "gerber_macro.h"
#include "gerber_command.h"
#include "parser/macro_parser/operator_item.h"

#include <array>
#include <cmath>


PrimitiveItem::PrimitiveItem() {
	type_ = kEndOfFile;
	index_ = 0;
}

PrimitiveItem::~PrimitiveItem() { // NOLINT
}

bool PrimitiveItem::Render(GerberMacro& macro) {
	switch (type_) {
	case PrimitiveItem::kCircle:
		if (!RenderCircle(macro)) {
			return false;
		}
		break;

	case PrimitiveItem::kLineVector:
	case PrimitiveItem::kLineVector2:
		if (!RenderLineVector(macro)) {
			return false;
		}
		break;

	case PrimitiveItem::kLineCenter:
		if (!RenderLineCenter(macro)) {
			return false;
		}
		break;

	case PrimitiveItem::kLineLowerLeft:
		if (!RenderLineLowerLeft(macro)) {
			return false;
		}
		break;

	case PrimitiveItem::kOutline:
		if (!RenderOutline(macro)) {
			return false;
		}
		break;

	case PrimitiveItem::kPolygon:
		if (!RenderPolygon(macro)) {
			return false;
		}
		break;

	case PrimitiveItem::kMoire:
		if (!RenderMoire(macro)) {
			return false;
		}
		break;

	case PrimitiveItem::kThermal:
		if (!RenderThermal(macro)) {
			return false;
		}
		break;

	case PrimitiveItem::kAssignment:
		if (!RenderAssignment(macro)) {
			return false;
		}
		break;

	default:
		break;
	}

	return true;
}

bool PrimitiveItem::RenderCircle(GerberMacro& macro) {
	constexpr int modifier_cnt = 5;
	std::array<double, modifier_cnt> modifier{};

	for (int j = 0; j < modifier_cnt; j++) {
		if (j < modifiers_.size()) {
			modifier[j] = Evaluate(modifiers_[j], macro.modifiers_);
		}
		else {
			modifier[j] = 0.0;
		}
	}

	if (modifier[0] == 0.0) {
		macro.exposure_ = false;
	}
	else if (modifier[0] == 1.0) {
		macro.exposure_ = true;
	}
	else {
		macro.exposure_ = !macro.exposure_;
	}

	double d = modifier[1];
	double x = modifier[2];
	double y = modifier[3];
	double a = modifier[4] * kPi / 180.0;// NOLINT
	double s = sin(a);
	double c = cos(a);

	macro.Add(std::make_shared<CircleCommand>(macro.Get_mm(c * x - s * y), macro.Get_mm(s * x + c * y), macro.Get_mm(d)));

	RenderCommand::Command cmd = RenderCommand::cErase;
	if (macro.exposure_) {
		cmd = RenderCommand::cFill;
	}

	macro.Add(std::make_shared<RenderCommand>(cmd));
	return true;
}

void PrimitiveItem::RenderLine(
	double x1, double y1,
	double x2, double y2,
	double x3, double y3,
	double x4, double y4,
	double x_r, double y_r,
	double angle, GerberMacro& macro
) {
	double r = 0.0;
	double a = 0.0;

	// Translate to center
	x1 -= x_r;
	y1 -= y_r;
	x2 -= x_r;
	y2 -= y_r;
	x3 -= x_r;
	y3 -= y_r;
	x4 -= x_r;
	y4 -= y_r;

	// Rotate the corners
	r = sqrt(y1 * y1 + x1 * x1);
	a = atan2(y1, x1) + angle;
	x1 = x_r + r * cos(a);
	y1 = y_r + r * sin(a);

	r = sqrt(y2 * y2 + x2 * x2);
	a = atan2(y2, x2) + angle;
	x2 = x_r + r * cos(a);
	y2 = y_r + r * sin(a);

	r = sqrt(y3 * y3 + x3 * x3);
	a = atan2(y3, x3) + angle;
	x3 = x_r + r * cos(a);
	y3 = y_r + r * sin(a);

	r = sqrt(y4 * y4 + x4 * x4);
	a = atan2(y4, x4) + angle;
	x4 = x_r + r * cos(a);
	y4 = y_r + r * sin(a);

	// Draw the line
	auto render = std::make_shared<BeginLineCommand>(macro.Get_mm(x1), macro.Get_mm(y1));
	macro.Add(render);

	auto render1 = std::make_shared<LineCommand>(macro.Get_mm(x2), macro.Get_mm(y2));
	macro.Add(render1);

	render1 = std::make_shared<LineCommand>(macro.Get_mm(x3), macro.Get_mm(y3));
	macro.Add(render1);

	render1 = std::make_shared<LineCommand>(macro.Get_mm(x4), macro.Get_mm(y4));
	macro.Add(render1);

	macro.Add(std::make_shared<CloseCommand>());
}

bool PrimitiveItem::RenderLineVector(GerberMacro& macro) {
	constexpr int modifier_count = 7;
	std::array<double, modifier_count> modifier{};

	for (int j = 0; j < modifier_count; j++) {
		if (j < modifiers_.size()) {
			modifier[j] = Evaluate(modifiers_[j], macro.modifiers_);
		}
		else {
			modifier[j] = 0.0;
		}
	}

	if (modifier[0] == 0.0) {
		macro.exposure_ = false;
	}
	else if (modifier[0] == 1.0) {
		macro.exposure_ = true;
	}
	else {
		macro.exposure_ = !macro.exposure_;
	}

	double w = 0.0;
	double a = 0.0; // Width; Angle
	double a1 = 0.0;
	double a2 = 0.0; // Angles of Start and End
	double r1 = 0.0;
	double r2 = 0.0; // Radii of Start and End
	double x1 = 0.0;
	double y1 = 0.0; // Start
	double x2 = 0.0;
	double y2 = 0.0; // End
	double x3 = 0.0;
	double y3 = 0.0; // Unit vector perpendicular to the line
	double x4 = 0.0;
	double y4 = 0.0; // Corner1
	double x5 = 0.0;
	double y5 = 0.0; // Corner2
	double x6 = 0.0;
	double y6 = 0.0; // Corner3
	double x7 = 0.0;
	double y7 = 0.0; // Corner4

	// Give the modifiers some names
	w = modifier[1];
	a = modifier[6] * kPi / 180.0;// NOLINT
	x1 = modifier[2];
	y1 = modifier[3];
	x2 = modifier[4];
	y2 = modifier[5];// NOLINT

	// Calculate the actual start and end points
	r1 = sqrt(y1 * y1 + x1 * x1);
	a1 = atan2(y1, x1) + a;
	x1 = r1 * cos(a1);
	y1 = r1 * sin(a1);

	r2 = sqrt(y2 * y2 + x2 * x2);
	a2 = atan2(y2, x2) + a;
	x2 = r2 * cos(a2);
	y2 = r2 * sin(a2);

	// Calculate the unit-vector perpendicular to the line
	a = atan2(y2 - y1, x2 - x1);
	a += kPi / 2.0;// NOLINT
	x3 = cos(a);
	y3 = sin(a);

	// Scale the unit vector with the line width
	w /= 2.0;// NOLINT
	x3 *= w;
	y3 *= w;

	// Calculate the corners
	x4 = x1 - x3;
	y4 = y1 - y3;
	x5 = x2 - x3;
	y5 = y2 - y3;
	x6 = x2 + x3;
	y6 = y2 + y3;
	x7 = x1 + x3;
	y7 = y1 + y3;

	// Draw the line
	auto render = std::make_shared<BeginLineCommand>(macro.Get_mm(x4), macro.Get_mm(y4));
	macro.Add(render);

	auto render1 = std::make_shared<LineCommand>(macro.Get_mm(x5), macro.Get_mm(y5));
	macro.Add(render1);

	render1 = std::make_shared<LineCommand>(macro.Get_mm(x6), macro.Get_mm(y6));
	macro.Add(render1);

	render1 = std::make_shared<LineCommand>(macro.Get_mm(x7), macro.Get_mm(y7));
	macro.Add(render1);

	macro.Add(std::make_shared<CloseCommand>());

	RenderCommand::Command cmd = RenderCommand::cErase;
	if (macro.exposure_) {
		cmd = RenderCommand::cFill;
	}

	macro.Add(std::make_shared<RenderCommand>(cmd));

	return true;
}

bool PrimitiveItem::RenderLineCenter(GerberMacro& macro) {
	constexpr int modifier_count = 6;
	std::array<double, modifier_count> modifier{};

	for (int j = 0; j < modifier_count; j++) {
		if (j < modifiers_.size()) {
			modifier[j] = Evaluate(modifiers_[j], macro.modifiers_);
		}
		else {
			modifier[j] = 0.0;
		}
	}

	if (modifier[0] == 0.0) {
		macro.exposure_ = false;
	}
	else if (modifier[0] == 1.0) {
		macro.exposure_ = true;
	}
	else {
		macro.exposure_ = !macro.exposure_;
	}

	double a = 0.0; // Rotation
	double w = 0.0;
	double h = 0.0; // Width and Height
	double x0 = 0.0;
	double y0 = 0.0; // Center
	double x1 = 0;
	double y1 = 0.0; // Corner 1
	double x2 = 0.0;
	double y2 = 0.0; // Corner 2
	double x3 = 0.0;
	double y3 = 0.0; // Corner 3
	double x4 = 0.0;
	double y4 = 0.0; // Corner 4

	// Give the modifiers some names
	w = modifier[1];
	h = modifier[2];
	x0 = modifier[3];
	y0 = modifier[4];
	a = modifier[5] * kPi / 180.0;// NOLINT

	// Calculate the corners without rotation
	w /= 2.0;// NOLINT
	h /= 2.0;// NOLINT

	x1 = x0 - w;
	y1 = y0 - h;
	x2 = x0 + w;
	y2 = y0 - h;
	x3 = x0 + w;
	y3 = y0 + h;
	x4 = x0 - w;
	y4 = y0 + h;

	RenderLine(x1, y1, x2, y2, x3, y3, x4, y4, 0, 0, a, macro);

	RenderCommand::Command cmd = RenderCommand::cErase;
	if (macro.exposure_) {
		cmd = RenderCommand::cFill;
	}

	macro.Add(std::make_shared<RenderCommand>(cmd));
	return true;
}

bool PrimitiveItem::RenderLineLowerLeft(GerberMacro& macro) {
	constexpr int modifier_count = 6;
	std::array<double, modifier_count> modifier{};

	for (int j = 0; j < modifier_count; j++) {
		if (j < modifiers_.size()) {
			modifier[j] = Evaluate(modifiers_[j], macro.modifiers_);
		}
		else {
			modifier[j] = 0.0;
		}
	}

	if (modifier[0] == 0.0) {
		macro.exposure_ = false;
	}
	else if (modifier[0] == 1.0) {
		macro.exposure_ = true;
	}
	else {
		macro.exposure_ = !macro.exposure_;
	}

	// Give the modifiers some names
	double w = modifier[1];
	double h = modifier[2];
	double x1 = modifier[3];
	double y1 = modifier[4];
	double a = modifier[5] * kPi / 180.0;// NOLINT

	// Calculate the corners without rotation
	double x2 = x1 + w;
	double y2 = y1;
	double x3 = x1 + w;
	double y3 = y1 + h;
	double x4 = x1;
	double y4 = y1 + h;

	RenderLine(x1, y1, x2, y2, x3, y3, x4, y4, 0, 0, a, macro);

	RenderCommand::Command cmd = RenderCommand::cErase;
	if (macro.exposure_) {
		cmd = RenderCommand::cFill;
	}

	macro.Add(std::make_shared<RenderCommand>(cmd));
	return true;
}

bool PrimitiveItem::RenderOutline(GerberMacro& macro) {
	const auto modifier_count = modifiers_.size();
	std::vector<double> modifier(modifier_count);

	for (int j = 0; j < modifier_count; j++) {
		modifier[j] = Evaluate(modifiers_[j], macro.modifiers_);
	}

	if (modifier[0] == 0.0) {
		macro.exposure_ = false;
	}
	else if (modifier[0] == 1.0) {
		macro.exposure_ = true;
	}
	else {
		macro.exposure_ = !macro.exposure_;
	}

	auto N = int((modifier_count - 5.0) / 2.0 + 1.0); // NOLINT

	double r = 0.0;
	double a = 0.0; // Temporary
	std::vector<double> x(N);
	std::vector<double> y(N);

	int j = 0;
	for (j = 0; j < N; j++) {
		x[j] = modifier[2 * j + 2];
		y[j] = modifier[2 * j + 3];
	}
	double A = modifier[2 * j + 2] * kPi / 180.0;// NOLINT

	// Make sure the polygon is closed
	x[N - 1] = x[0];
	y[N - 1] = y[0];

	// Rotate the points
	for (int j = 0; j < N; j++) {
		r = sqrt(y[j] * y[j] + x[j] * x[j]);
		a = atan2(y[j], x[j]) + A;
		x[j] = r * cos(a);
		y[j] = r * sin(a);
	}

	// Determine the wind
	double Area = 0.0; // Actually double the area
	for (int j = 0; j < N - 1; j++) {
		Area += (x[j] - x[j + 1]) * (y[j] + y[j + 1]);
	}

	bool CCW = false;
	if (Area > 0.0) {
		CCW = true;
	}

	if (CCW) {
		auto render = std::make_shared<BeginLineCommand>(macro.Get_mm(x[0]), macro.Get_mm(y[0]));
		macro.Add(render);

		for (int j = 1; j < N - 1; j++) {
			auto render = std::make_shared<LineCommand>(macro.Get_mm(x[j]), macro.Get_mm(y[j]));
			macro.Add(render);
		}

	}
	else {
		auto render = std::make_shared<BeginLineCommand>(macro.Get_mm(x[N - 1]), macro.Get_mm(y[N - 1]));
		macro.Add(render);

		for (int j = N - 2; j > 0; j--) {
			auto render = std::make_shared<LineCommand>(macro.Get_mm(x[j]), macro.Get_mm(y[j]));
			macro.Add(render);
		}
	}

	macro.Add(std::make_shared<CloseCommand>());

	RenderCommand::Command cmd = RenderCommand::cErase;
	if (macro.exposure_) {
		cmd = RenderCommand::cFill;
	}

	macro.Add(std::make_shared<RenderCommand>(cmd));
	return true;
}

bool PrimitiveItem::RenderPolygon(GerberMacro& macro) {
	constexpr int modifier_count = 6;
	std::array<double, modifier_count> modifier{};

	for (int j = 0; j < modifier_count; j++) {
		if (j < modifiers_.size()) {
			modifier[j] = Evaluate(modifiers_[j], macro.modifiers_);
		}
		else {
			modifier[j] = 0.0;
		}
	}

	if (modifier[0] == 0.0) {
		macro.exposure_ = false;
	}
	else if (modifier[0] == 1.0) {
		macro.exposure_ = true;
	}
	else {
		macro.exposure_ = !macro.exposure_;
	}

	int N = int(modifier[1]);
	double X = modifier[2];
	double Y = modifier[3];
	double R = modifier[4] / 2.0;// NOLINT
	double A = modifier[5] * kPi / 180.0;// NOLINT

	std::vector<double> x(N);
	std::vector<double> y(N);

	double da = 2.0 * kPi / N;// NOLINT

	double r = sqrt(Y * Y + X * X);
	double a = atan2(Y, X) + A;
	X = r * cos(a);
	Y = r * sin(a);

	a = A;
	for (int j = 0; j < N; j++) {
		x[j] = X + R * cos(a);
		y[j] = Y + R * sin(a);
		a += da;
	}

	auto render = std::make_shared<BeginLineCommand>(macro.Get_mm(x[0]), macro.Get_mm(y[0]));
	macro.Add(render);

	for (int j = 1; j < N; j++) {
		auto render = std::make_shared<LineCommand>(macro.Get_mm(x[j]), macro.Get_mm(y[j]));
		macro.Add(render);
	}

	macro.Add(std::make_shared<CloseCommand>());

	RenderCommand::Command cmd = RenderCommand::cErase;
	if (macro.exposure_) {
		cmd = RenderCommand::cFill;
	}

	macro.Add(std::make_shared<RenderCommand>(cmd));
	return true;
}

bool PrimitiveItem::RenderMoire(GerberMacro& macro) {
	constexpr int modifier_count = 9;
	std::array<double, modifier_count> modifier{};

	for (int j = 0; j < modifier_count; j++) {
		if (j < modifiers_.size()) {
			modifier[j] = Evaluate(modifiers_[j], macro.modifiers_);
		}
		else {
			modifier[j] = 0.0;
		}
	}

	auto X = modifier[0];
	auto Y = modifier[1];
	auto OD = modifier[2];
	auto thickness = modifier[3];
	auto gap = modifier[4];
	int N = int(modifier[5]);  // NOLINT

	auto width = modifier[6];// NOLINT
	auto length = modifier[7];// NOLINT
	auto rotation = modifier[8];// NOLINT

	// Draw rings:
	double d = OD;
	for (int j = 0; j < N; j++) {
		if (d < width)
		{
			break;
		}

		macro.Add(std::make_shared<CircleCommand>(macro.Get_mm(X), macro.Get_mm(Y), macro.Get_mm(d)));

		d -= thickness * 2.0;// NOLINT
		if (d < width) {
			break;
		}

		macro.Add(std::make_shared<BeginLineCommand>(macro.Get_mm(X + d / 2.0), macro.Get_mm(Y)));
		macro.Add(std::make_shared<ArcCommand>(macro.Get_mm(X), macro.Get_mm(Y), -360.0));

		d -= gap * 2.0;// NOLINT
	}

	auto dx = width / 2.0;// NOLINT
	auto dy = length / 2.0;// NOLINT

	auto x1 = -dx;
	auto y1 = -dy;
	auto x2 = +dx;
	auto y2 = -dy;
	auto x3 = +dx;
	auto y3 = +dy;
	auto x4 = -dx;
	auto y4 = +dy;

	RenderLine(
		X + x1, Y + y1,
		X + x2, Y + y2,
		X + x3, Y + y3,
		X + x4, Y + y4,
		X, Y,
		rotation, macro
	);
	RenderLine(
		X + x1, Y + y1,
		X + x2, Y + y2,
		X + x3, Y + y3,
		X + x4, Y + y4,
		X, Y,
		rotation + kPi / 2.0, macro // NOLINT
	);

	macro.Add(std::make_shared<FillCommand>());

	return true;
}

bool PrimitiveItem::RenderThermal(GerberMacro& macro) {
	constexpr int modifier_count = 6;
	std::array<double, modifier_count> modifier{};

	for (int j = 0; j < modifier_count; j++) {
		if (j < modifiers_.size()) {
			modifier[j] = Evaluate(modifiers_[j], macro.modifiers_);
		}
		else {
			modifier[j] = 0.0;
		}
	}

	double X = modifier[0];
	double Y = modifier[1];
	double OD = modifier[2];
	double ID = modifier[3];
	double Gap = modifier[4];
	double Rot = modifier[5];// NOLINT

	OD /= 2.0;// NOLINT
	ID /= 2.0;// NOLINT
	Gap /= 2.0;// NOLINT
	Rot *= kPi / 180.0;// NOLINT

	double x1 = sqrt(OD * OD - Gap * Gap);
	double y1 = Gap;
	double x2 = Gap;
	double y2 = sqrt(ID * ID - Gap * Gap);

	double a1 = -2.0 * atan2(y1, x1) + kPi / 2.0;// NOLINT
	double a2 = 2.0 * atan2(y2, x2) - kPi / 2.0;// NOLINT

	double r = sqrt(y1 * y1 + x1 * x1);
	double t = atan2(y1, x1) + Rot;
	x1 = r * cos(t);
	y1 = r * sin(t);

	r = sqrt(y2 * y2 + x2 * x2);
	t = atan2(y2, x2) + Rot;
	x2 = r * cos(t);
	y2 = r * sin(t);

	a1 *= 180.0 / kPi;// NOLINT
	a2 *= 180.0 / kPi;// NOLINT

	for (int j = 0; j < 4; j++) {
		macro.Add(std::make_shared<BeginLineCommand>(macro.Get_mm(X + x1), macro.Get_mm(Y + y1)));
		macro.Add(std::make_shared<ArcCommand>(macro.Get_mm(X), macro.Get_mm(Y), a1));
		macro.Add(std::make_shared<LineCommand>(macro.Get_mm(X + x2), macro.Get_mm(Y + y2)));
		macro.Add(std::make_shared<ArcCommand>(macro.Get_mm(X), macro.Get_mm(Y), -a2));
		macro.Add(std::make_shared<CloseCommand>());

		// Rotate 90 deg
		t = x1;
		x1 = -y1;
		y1 = t;
		t = x2;
		x2 = -y2;
		y2 = t;
	}

	macro.Add(std::make_shared<FillCommand>());
	return true;
}

bool PrimitiveItem::RenderAssignment(GerberMacro& macro) {
	if (modifiers_.empty()) {
		return false;
	}

	if (index_ > macro.modifiers_.size()) {
		macro.modifiers_.resize(index_);
	}

	macro.modifiers_[index_ - 1] = Evaluate(modifiers_[0], macro.modifiers_);
	return true;
}


double PrimitiveItem::Evaluate(const std::shared_ptr<OperatorItem>& root, const std::vector<double>& modifiers) {
	if (!root) {
		return 0.0;
	}

	switch (root->Opera()) {
	case OperatorItem::kAdd:
		return Evaluate(root->Left(), modifiers) + Evaluate(root->Right(), modifiers);

	case OperatorItem::kSubtract:
		return Evaluate(root->Left(), modifiers) - Evaluate(root->Right(), modifiers);

	case OperatorItem::kMultiply:
		return Evaluate(root->Left(), modifiers) * Evaluate(root->Right(), modifiers);

	case OperatorItem::kDivide:
		return Evaluate(root->Left(), modifiers) / Evaluate(root->Right(), modifiers);

	case OperatorItem::kVariable:
		if (root->Index > 0 && root->Index <= modifiers.size()) {
			return modifiers[root->Index - 1];
		}
		else {
			return 0.0;
		}

	case OperatorItem::kLiteral:
		return root->Value;

	default:
		return 0.0;
	}
}
