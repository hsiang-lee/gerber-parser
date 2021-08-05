#include "gerber_macro.h"
#include <glog/logging.h>


constexpr double kPi = 3.141592653589793238463;


GerberMacro::GerberMacro() {
	Primitives = 0;
	PrimitivesLast = 0;
	Modifiers = 0;
	Inches = true;
	exposure_ = true;
	NewModifiers = false;
}


GerberMacro::~GerberMacro() {
	if (Primitives) delete   Primitives;
	if (NewModifiers) delete[] Modifiers;
}


GerberMacro::OPERATOR_ITEM::OPERATOR_ITEM() {
	Operator = opLiteral;
	left_ = 0;
	right_ = 0;
	Index = 0;
	Value = 0;
}


GerberMacro::OPERATOR_ITEM::~OPERATOR_ITEM() {
	if (left_) delete left_;
	if (right_) delete right_;
}


GerberMacro::PRIMITIVE_ITEM::PRIMITIVE_ITEM() {
	Primitive = pEndOfFile;
	Modifier = 0;
	ModifierCount = 0;
	Index = 0;
	Next = 0;
}


GerberMacro::PRIMITIVE_ITEM::~PRIMITIVE_ITEM() {
	int j;
	for (j = 0; j < ModifierCount; j++) {
		if (Modifier[j]) delete Modifier[j];
	}

	if (Next) delete Next; // Hopefully this is a short list :-)
}


void GerberMacro::Add(std::shared_ptr<RenderCommand> Render) {
	render_commands_.push_back(Render);
}


double GerberMacro::Evaluate(OPERATOR_ITEM* Root) {
	double left, right;

	if (!Root) return 0.0;

	switch (Root->Operator) {
	case opAdd:
		return Evaluate(Root->left_) + Evaluate(Root->right_);

	case opSubtract:
		return Evaluate(Root->left_) - Evaluate(Root->right_);

	case opMultiply:
		return Evaluate(Root->left_) * Evaluate(Root->right_);

	case opDivide:
		return Evaluate(Root->left_) / Evaluate(Root->right_);

	case opVariable:
		if (Root->Index > 0 && Root->Index <= ModifierCount) {
			return Modifiers[Root->Index - 1];
		}
		else {
			return 0.0;
		}

	case opLiteral:
		return Root->Value;

	default:
		return 0.0;
	}
}


double GerberMacro::Get_mm(double number) {
	if (Inches)
		number *= 25.4;

	return number;
}


void GerberMacro::RenderLine(
	double x1, double y1,
	double x2, double y2,
	double x3, double y3,
	double x4, double y4,
	double xR, double yR,
	double A
) {
	double r, a;

	// Translate to center
	x1 -= xR;
	y1 -= yR;
	x2 -= xR;
	y2 -= yR;
	x3 -= xR;
	y3 -= yR;
	x4 -= xR;
	y4 -= yR;

	// Rotate the corners
	r = sqrt(y1 * y1 + x1 * x1);
	a = atan2(y1, x1) + A;
	x1 = xR + r * cos(a);
	y1 = yR + r * sin(a);

	r = sqrt(y2 * y2 + x2 * x2);
	a = atan2(y2, x2) + A;
	x2 = xR + r * cos(a);
	y2 = yR + r * sin(a);

	r = sqrt(y3 * y3 + x3 * x3);
	a = atan2(y3, x3) + A;
	x3 = xR + r * cos(a);
	y3 = yR + r * sin(a);

	r = sqrt(y4 * y4 + x4 * x4);
	a = atan2(y4, x4) + A;
	x4 = xR + r * cos(a);
	y4 = yR + r * sin(a);

	// Draw the line
	auto render = std::make_shared<RenderCommand>(RenderCommand::gcBeginLine);
	render->X = Get_mm(x1);
	render->Y = Get_mm(y1);
	Add(render);

	render = std::make_shared<RenderCommand>(RenderCommand::gcLine);
	render->X = Get_mm(x2);
	render->Y = Get_mm(y2);
	Add(render);

	render = std::make_shared<RenderCommand>(RenderCommand::gcLine);
	render->X = Get_mm(x3);
	render->Y = Get_mm(y3);
	Add(render);

	render = std::make_shared<RenderCommand>(RenderCommand::gcLine);
	render->X = Get_mm(x4);
	render->Y = Get_mm(y4);
	Add(render);

	render = std::make_shared<RenderCommand>(RenderCommand::gcClose);
	Add(render);
}


bool GerberMacro::RenderCircle(PRIMITIVE_ITEM* Primitive) {
	constexpr int modifier_cnt = 5;
	double modifier[modifier_cnt];

	for (int j = 0; j < modifier_cnt; j++) {
		if (j < Primitive->ModifierCount) {
			modifier[j] = Evaluate(Primitive->Modifier[j]);
		}
		else {
			modifier[j] = 0.0;
		}
	}

	if (modifier[0] == 0.0) {
		exposure_ = false;
	}
	else if (modifier[0] == 1.0) {
		exposure_ = true;
	}
	else {
		exposure_ = !exposure_;
	}

	double d = modifier[1];
	double x = modifier[2];
	double y = modifier[3];
	double a = modifier[4] * kPi / 180.0;
	double s = sin(a);
	double c = cos(a);

	auto render = std::make_shared<RenderCommand>(RenderCommand::gcCircle);
	render->X = Get_mm(c * x - s * y);
	render->Y = Get_mm(s * x + c * y);
	render->W = Get_mm(d);
	Add(render);

	RenderCommand::GerberCommand cmd;
	if (exposure_) {
		cmd = RenderCommand::gcFill;
	}
	else {
		cmd = RenderCommand::gcErase;
	}
	render = std::make_shared<RenderCommand>(cmd);

	Add(render);

	return true;
}


bool GerberMacro::RenderLineVector(PRIMITIVE_ITEM* Primitive) {
	constexpr int modifier_count = 7;
	double modifier[modifier_count];

	for (int j = 0; j < modifier_count; j++) {
		if (j < Primitive->ModifierCount) {
			modifier[j] = Evaluate(Primitive->Modifier[j]);
		}
		else {
			modifier[j] = 0.0;
		}
	}

	if (modifier[0] == 0.0) {
		exposure_ = false;
	}
	else if (modifier[0] == 1.0) {
		exposure_ = true;
	}
	else {
		exposure_ = !exposure_;
	}

	double w, a; // Width; Angle
	double a1, a2; // Angles of Start and End
	double r1, r2; // Radii of Start and End
	double x1, y1; // Start
	double x2, y2; // End
	double x3, y3; // Unit vector perpendicular to the line
	double x4, y4; // Corner1
	double x5, y5; // Corner2
	double x6, y6; // Corner3
	double x7, y7; // Corner4

	// Give the modifiers some names
	w = modifier[1];
	a = modifier[6] * kPi / 180.0;
	x1 = modifier[2];
	y1 = modifier[3];
	x2 = modifier[4];
	y2 = modifier[5];

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
	a += kPi / 2.0;
	x3 = cos(a);
	y3 = sin(a);

	// Scale the unit vector with the line width
	w /= 2.0;
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
	auto render = std::make_shared<RenderCommand>(RenderCommand::gcBeginLine);
	render->X = Get_mm(x4);
	render->Y = Get_mm(y4);
	Add(render);

	render = std::make_shared<RenderCommand>(RenderCommand::gcLine);
	render->X = Get_mm(x5);
	render->Y = Get_mm(y5);
	Add(render);

	render = std::make_shared<RenderCommand>(RenderCommand::gcLine);
	render->X = Get_mm(x6);
	render->Y = Get_mm(y6);
	Add(render);

	render = std::make_shared<RenderCommand>(RenderCommand::gcLine);
	render->X = Get_mm(x7);
	render->Y = Get_mm(y7);
	Add(render);

	render = std::make_shared<RenderCommand>(RenderCommand::gcClose);
	Add(render);

	RenderCommand::GerberCommand cmd;
	if (exposure_) {
		cmd = RenderCommand::gcFill;
	}
	else {
		cmd = RenderCommand::gcErase;
	}
	render = std::make_shared<RenderCommand>(cmd);
	Add(render);

	return true;
}


bool GerberMacro::RenderLineCenter(PRIMITIVE_ITEM* Primitive) {
	constexpr int modifier_count = 6;
	double modifier[modifier_count];

	for (int j = 0; j < modifier_count; j++) {
		if (j < Primitive->ModifierCount) {
			modifier[j] = Evaluate(Primitive->Modifier[j]);
		}
		else {
			modifier[j] = 0.0;
		}
	}

	if (modifier[0] == 0.0) {
		exposure_ = false;
	}
	else if (modifier[0] == 1.0) {
		exposure_ = true;
	}
	else {
		exposure_ = !exposure_;
	}

	double a; // Rotation
	double w, h; // Width and Height
	double x0, y0; // Center
	double x1, y1; // Corner 1
	double x2, y2; // Corner 2
	double x3, y3; // Corner 3
	double x4, y4; // Corner 4

	// Give the modifiers some names
	w = modifier[1];
	h = modifier[2];
	x0 = modifier[3];
	y0 = modifier[4];
	a = modifier[5] * kPi / 180.0;

	// Calculate the corners without rotation
	w /= 2.0;
	h /= 2.0;

	x1 = x0 - w;
	y1 = y0 - h;
	x2 = x0 + w;
	y2 = y0 - h;
	x3 = x0 + w;
	y3 = y0 + h;
	x4 = x0 - w;
	y4 = y0 + h;

	RenderLine(x1, y1, x2, y2, x3, y3, x4, y4, 0, 0, a);

	RenderCommand::GerberCommand cmd;
	if (exposure_) {
		cmd = RenderCommand::gcFill;
	}
	else {
		cmd = RenderCommand::gcErase;
	}
	auto render = std::make_shared<RenderCommand>(cmd);
	Add(render);

	return true;
}


bool GerberMacro::RenderLineLowerLeft(PRIMITIVE_ITEM* primitive) {
	constexpr int modifier_count = 6;
	double modifier[modifier_count];

	for (int j = 0; j < modifier_count; j++) {
		if (j < primitive->ModifierCount) {
			modifier[j] = Evaluate(primitive->Modifier[j]);
		}
		else {
			modifier[j] = 0.0;
		}
	}

	if (modifier[0] == 0.0) {
		exposure_ = false;
	}
	else if (modifier[0] == 1.0) {
		exposure_ = true;
	}
	else {
		exposure_ = !exposure_;
	}

	double a; // Rotation
	double w, h; // Lower Left
	double x1, y1; // Corner 1
	double x2, y2; // Corner 2
	double x3, y3; // Corner 3
	double x4, y4; // Corner 4

	// Give the modifiers some names
	w = modifier[1];
	h = modifier[2];
	x1 = modifier[3];
	y1 = modifier[4];
	a = modifier[5] * kPi / 180.0;

	// Calculate the corners without rotation
	x2 = x1 + w;
	y2 = y1;
	x3 = x1 + w;
	y3 = y1 + h;
	x4 = x1;
	y4 = y1 + h;

	RenderLine(x1, y1, x2, y2, x3, y3, x4, y4, 0, 0, a);

	RenderCommand::GerberCommand cmd;
	if (exposure_) {
		cmd = RenderCommand::gcFill;
	}
	else {
		cmd = RenderCommand::gcErase;
	}
	auto render = std::make_shared<RenderCommand>(cmd);
	Add(render);

	return true;
}


bool GerberMacro::RenderOutline(PRIMITIVE_ITEM* Primitive) {
	std::shared_ptr<RenderCommand> render;

	const int modifier_count = Primitive->ModifierCount;
	double* modifier = new double[modifier_count];

	for (int j = 0; j < modifier_count; j++) {
		modifier[j] = Evaluate(Primitive->Modifier[j]);
	}

	if (modifier[0] == 0.0) {
		exposure_ = false;
	}
	else if (modifier[0] == 1.0) {
		exposure_ = true;
	}
	else {
		exposure_ = !exposure_;
	}

	int N = (modifier_count - 5.0) / 2.0 + 1.0; // Total number of points
	double  A; // Rotation;

	double  r, a; // Temporary
	double* x = new double[N]; // Points on polygon
	double* y = new double[N];

	int j = 0;
	for (j = 0; j < N; j++) {
		x[j] = modifier[2 * j + 2];
		y[j] = modifier[2 * j + 3];
	}
	A = modifier[2 * j + 2] * kPi / 180.0;

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
	bool   CCW;
	double Area = 0.0; // Actually double the area
	for (int j = 0; j < N - 1; j++) {
		Area += (x[j] - x[j + 1]) * (y[j] + y[j + 1]);
	}

	if (Area > 0.0) {
		CCW = true;
	}
	else {
		CCW = false;
	}

	if (CCW) {
		render = std::make_shared<RenderCommand>(RenderCommand::gcBeginLine);
		render->X = Get_mm(x[0]);
		render->Y = Get_mm(y[0]);
		Add(render);

		for (int j = 1; j < N - 1; j++) {
			render = std::make_shared<RenderCommand>(RenderCommand::gcLine);
			render->X = Get_mm(x[j]);
			render->Y = Get_mm(y[j]);
			Add(render);
		}

	}
	else {
		render = std::make_shared<RenderCommand>(RenderCommand::gcBeginLine);
		render->X = Get_mm(x[N - 1]);
		render->Y = Get_mm(y[N - 1]);
		Add(render);

		for (int j = N - 2; j > 0; j--) {
			render = std::make_shared<RenderCommand>(RenderCommand::gcLine);
			render->X = Get_mm(x[j]);
			render->Y = Get_mm(y[j]);
			Add(render);
		}
	}

	render = std::make_shared<RenderCommand>(RenderCommand::gcClose);
	Add(render);

	RenderCommand::GerberCommand cmd;
	if (exposure_) {
		cmd = RenderCommand::gcFill;
	}
	else {
		cmd = RenderCommand::gcErase;
	}
	render = std::make_shared<RenderCommand>(cmd);
	Add(render);

	delete[] x;
	delete[] y;
	delete[] modifier;

	return true;
}


bool GerberMacro::RenderPolygon(PRIMITIVE_ITEM* primitive) {
	const int modifier_count = 6;
	double modifier[modifier_count];

	for (int j = 0; j < modifier_count; j++) {
		if (j < primitive->ModifierCount) {
			modifier[j] = Evaluate(primitive->Modifier[j]);
		}
		else {
			modifier[j] = 0.0;
		}
	}

	if (modifier[0] == 0.0) {
		exposure_ = false;
	}
	else if (modifier[0] == 1.0) {
		exposure_ = true;
	}
	else {
		exposure_ = !exposure_;
	}

	int     N;
	double  X, Y, R, A;

	N = modifier[1];
	X = modifier[2];
	Y = modifier[3];
	R = modifier[4] / 2.0;
	A = modifier[5] * kPi / 180.0;

	double  r, a, da;
	double* x = new double[N];
	double* y = new double[N];

	da = 2.0 * kPi / N;

	r = sqrt(Y * Y + X * X);
	a = atan2(Y, X) + A;
	X = r * cos(a);
	Y = r * sin(a);

	a = A;
	for (int j = 0; j < N; j++) {
		x[j] = X + R * cos(a);
		y[j] = Y + R * sin(a);
		a += da;
	}

	auto render = std::make_shared<RenderCommand>(RenderCommand::gcBeginLine);
	render->X = Get_mm(x[0]);
	render->Y = Get_mm(y[0]);
	Add(render);

	for (int j = 1; j < N; j++) {
		render = std::make_shared<RenderCommand>(RenderCommand::gcLine);
		render->X = Get_mm(x[j]);
		render->Y = Get_mm(y[j]);
		Add(render);
	}

	render = std::make_shared<RenderCommand>(RenderCommand::gcClose);
	Add(render);

	RenderCommand::GerberCommand cmd;
	if (exposure_) {
		cmd = RenderCommand::gcFill;
	}
	else {
		cmd = RenderCommand::gcErase;
	}
	render = std::make_shared<RenderCommand>(cmd);
	Add(render);

	delete[] x;
	delete[] y;

	return true;
}


bool GerberMacro::RenderMoire(PRIMITIVE_ITEM* primitive) {
	std::shared_ptr<RenderCommand> render;

	const int modifier_count = 9;
	double modifier[modifier_count];

	for (int j = 0; j < modifier_count; j++) {
		if (j < primitive->ModifierCount) {
			modifier[j] = Evaluate(primitive->Modifier[j]);
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
	int N = modifier[5];

	auto width = modifier[6];
	auto length = modifier[7];
	auto rotation = modifier[8];

	// Draw rings:
	double d = OD;
	for (int j = 0; j < N; j++) {
		if (d < width) break;
		render = std::make_shared<RenderCommand>(RenderCommand::gcCircle);
		render->X = Get_mm(X);
		render->Y = Get_mm(Y);
		render->W = Get_mm(d);
		Add(render);

		d -= thickness * 2.0;
		if (d < width) break;
		render = std::make_shared<RenderCommand>(RenderCommand::gcBeginLine);
		render->X = Get_mm(X + d / 2.0);
		render->Y = Get_mm(Y);
		Add(render);
		render = std::make_shared<RenderCommand>(RenderCommand::gcArc);
		render->X = Get_mm(X);
		render->Y = Get_mm(Y);
		render->A = -360.0;
		Add(render);

		d -= gap * 2.0;
	}

	auto dx = width / 2.0;
	auto dy = length / 2.0;

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
		rotation
	);
	RenderLine(
		X + x1, Y + y1,
		X + x2, Y + y2,
		X + x3, Y + y3,
		X + x4, Y + y4,
		X, Y,
		rotation + kPi / 2.0
	);

	render = std::make_shared<RenderCommand>(RenderCommand::gcFill);
	Add(render);

	return true;
}


bool GerberMacro::RenderThermal(PRIMITIVE_ITEM* Primitive) {
	std::shared_ptr<RenderCommand> render;

	const int ModifierCount = 6;
	double Modifier[ModifierCount];

	for (int j = 0; j < ModifierCount; j++) {
		if (j < Primitive->ModifierCount) {
			Modifier[j] = Evaluate(Primitive->Modifier[j]);
		}
		else {
			Modifier[j] = 0.0;
		}
	}

	double X, Y;
	double OD;
	double ID;
	double Gap;
	double Rot;

	X = Modifier[0];
	Y = Modifier[1];
	OD = Modifier[2];
	ID = Modifier[3];
	Gap = Modifier[4];
	Rot = Modifier[5];

	double x1, y1;
	double x2, y2;
	double a1, a2; // Inner and outer angles
	double r, t;

	OD /= 2.0;
	ID /= 2.0;
	Gap /= 2.0;
	Rot *= kPi / 180.0;

	x1 = sqrt(OD * OD - Gap * Gap);
	y1 = Gap;
	x2 = Gap;
	y2 = sqrt(ID * ID - Gap * Gap);

	a1 = -2.0 * atan2(y1, x1) + kPi / 2.0;
	a2 = 2.0 * atan2(y2, x2) - kPi / 2.0;

	r = sqrt(y1 * y1 + x1 * x1);
	t = atan2(y1, x1) + Rot;
	x1 = r * cos(t);
	y1 = r * sin(t);

	r = sqrt(y2 * y2 + x2 * x2);
	t = atan2(y2, x2) + Rot;
	x2 = r * cos(t);
	y2 = r * sin(t);

	a1 *= 180.0 / kPi;
	a2 *= 180.0 / kPi;

	for (int j = 0; j < 4; j++) {
		render = std::make_shared<RenderCommand>(RenderCommand::gcBeginLine);
		render->X = Get_mm(X + x1);
		render->Y = Get_mm(Y + y1);
		Add(render);

		render = std::make_shared<RenderCommand>(RenderCommand::gcArc);
		render->X = Get_mm(X);
		render->Y = Get_mm(Y);
		render->A = a1;
		Add(render);

		render = std::make_shared<RenderCommand>(RenderCommand::gcLine);
		render->X = Get_mm(X + x2);
		render->Y = Get_mm(Y + y2);
		Add(render);

		render = std::make_shared<RenderCommand>(RenderCommand::gcArc);
		render->X = Get_mm(X);
		render->Y = Get_mm(Y);
		render->A = -a2;
		Add(render);

		render = std::make_shared<RenderCommand>(RenderCommand::gcClose);
		Add(render);

		// Rotate 90 deg
		t = x1;
		x1 = -y1;
		y1 = t;
		t = x2;
		x2 = -y2;
		y2 = t;
	}

	render = std::make_shared<RenderCommand>(RenderCommand::gcFill);
	Add(render);

	return true;
}


bool GerberMacro::RenderAssignment(PRIMITIVE_ITEM* Primitive) {
	int     j, t;
	double* Temp;

	if (Primitive->ModifierCount < 1) return false;

	if (Primitive->Index > ModifierCount) {
		t = ModifierCount;
		ModifierCount = Primitive->Index;

		Temp = new double[ModifierCount];
		for (j = 0; j < t; j++) {
			Temp[j] = Modifiers[j];
		}
		for (; j < ModifierCount; j++) {
			Temp[j] = 0.0;
		}

		if (NewModifiers) delete[] Modifiers;
		Modifiers = Temp;
		NewModifiers = true;
	}

	Modifiers[Primitive->Index - 1] = Evaluate(Primitive->Modifier[0]);

	return true;
}


std::vector<std::shared_ptr<RenderCommand>> GerberMacro::Render(double* Modifiers, int ModifierCount) {
	render_commands_.clear();
	if (NewModifiers) delete[] GerberMacro::Modifiers;
	NewModifiers = false;

	exposure_ = true;

	GerberMacro::Modifiers = Modifiers;
	GerberMacro::ModifierCount = ModifierCount;

	PRIMITIVE_ITEM* Primitive = Primitives;
	while (Primitive) {
		switch (Primitive->Primitive) {
		case pCircle:
			if (!RenderCircle(Primitive)) return {};
			break;

		case pLineVector:
		case pLineVector2:
			if (!RenderLineVector(Primitive)) return {};
			break;

		case pLineCenter:
			if (!RenderLineCenter(Primitive)) return {};
			break;

		case pLineLowerLeft:
			if (!RenderLineLowerLeft(Primitive)) return {};
			break;

		case pOutline:
			if (!RenderOutline(Primitive)) return {};
			break;

		case pPolygon:
			if (!RenderPolygon(Primitive)) return {};
			break;

		case pMoire:
			if (!RenderMoire(Primitive)) return {};
			break;

		case pThermal:
			if (!RenderThermal(Primitive)) return {};
			break;

		case pAssignment:
			if (!RenderAssignment(Primitive)) return {};
			break;

		default:
			break;
		}
		Primitive = Primitive->Next;
	}

	return render_commands_;
}


void GerberMacro::Add(PRIMITIVE_ITEM* Primitive) {
	if (!Primitive) return;

	if (Primitives) {
		PrimitivesLast->Next = Primitive;
	}
	else {
		Primitives = Primitive;
	}

	PrimitivesLast = Primitive;
}


bool GerberMacro::Float(double* Number) {
	int       Integer = 0;
	bool      Sign = false;
	double    Scale = 0.1;
	unsigned  i = Index;

	if (Index < Length && Buffer[Index] == '-') {
		Sign = true;
		Index++;
	}
	else if (Index < Length && Buffer[Index] == '+') {
		Index++;
	}

	while (Index < Length) {
		if (Buffer[Index] >= '0' && Buffer[Index] <= '9') {
			Integer *= 10;
			Integer += Buffer[Index] - '0';
			Index++;
		}
		else {
			break;
		}
	}

	*Number = Integer;

	if (Index < Length && Buffer[Index] == '.') {
		Index++;
		while (Index < Length) {
			if (Buffer[Index] >= '0' && Buffer[Index] <= '9') {
				*Number += (Buffer[Index] - '0') * Scale;
				Scale *= 0.1;
				Index++;
			}
			else {
				break;
			}
		}
	}

	if (Index < Length) {
		if (Sign) *Number *= -1.0;
		return (Index > i);
	}

	Index = i;
	return false;
}


bool GerberMacro::Integer(int* Integer) {
	bool     Sign = false;
	unsigned i = Index;

	*Integer = 0;

	if (Index < Length && Buffer[Index] == '-') {
		Sign = true;
		Index++;
	}
	else if (Index < Length && Buffer[Index] == '+') {
		Index++;
	}

	while (Index < Length) {
		if (Buffer[Index] >= '0' && Buffer[Index] <= '9') {
			*Integer *= 10;
			*Integer += Buffer[Index] - '0';
			Index++;
		}
		else {
			if (Sign) *Integer *= -1;
			return (Index > i);
		}
	}

	Index = i;
	return false;
}


void GerberMacro::SkipWhiteSpace() {
	while (Index < Length) {
		switch (Buffer[Index]) {
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			break;

		default:
			return;
		}
		Index++;
	}
}


// Term {("+" | "-") Term};
GerberMacro::OPERATOR_ITEM* GerberMacro::Modifier() {
	OPERATOR       Operator;
	OPERATOR_ITEM* left_;
	OPERATOR_ITEM* Root;

	SkipWhiteSpace();

	left_ = Term();
	if (!left_) return 0;

	while (Index < Length) {
		switch (Buffer[Index]) {
		case '+':
			Operator = opAdd;
			break;

		case '-':
			Operator = opSubtract;
			break;

		default:
			return left_;
		}
		Index++;

		Root = new OPERATOR_ITEM;
		Root->Operator = Operator;
		Root->left_ = left_;
		Root->right_ = Term();

		left_ = Root;
	}

	delete left_;

	return 0;
}


// Factor {("x" | "X" | "/") Factor};
GerberMacro::OPERATOR_ITEM* GerberMacro::Term() {
	OPERATOR       Operator;
	OPERATOR_ITEM* left_;
	OPERATOR_ITEM* Root;

	SkipWhiteSpace();

	left_ = Factor();
	if (!left_) return 0;

	while (Index < Length) {
		switch (Buffer[Index]) {
		case 'x':
		case 'X':
			Operator = opMultiply;
			break;

		case '/':
			Operator = opDivide;
			break;

		default:
			return left_;
		}
		Index++;

		Root = new OPERATOR_ITEM;
		Root->Operator = Operator;
		Root->left_ = left_;
		Root->right_ = Factor();

		left_ = Root;
	}

	delete left_;

	return 0;
}


// ["+" | "-"] (("(" Modifier ")") | Variable | Float)
GerberMacro::OPERATOR_ITEM* GerberMacro::Factor() {
	bool           negative_ = false;
	double         d;
	OPERATOR_ITEM* Item;

	SkipWhiteSpace();

	if (Index >= Length) return 0;
	if (Buffer[Index] == '-') {
		negative_ = true;
		Index++; SkipWhiteSpace();

	}
	else if (Buffer[Index] == '+') {
		Index++; SkipWhiteSpace();
	}

	if (Buffer[Index] == '(') {
		Index++;

		Item = Modifier();
		if (!Item) {
			LOG(ERROR) << "Error: Expression expected";
			return 0;
		}

		SkipWhiteSpace();

		if (Buffer[Index] != ')') {
			LOG(ERROR) << "Error: ')' expected";
			delete Item;
			return 0;
		}
		Index++;

	}
	else {
		Item = Variable();
		if (!Item) {
			if (!Float(&d)) {
				LOG(ERROR) << "Error: Float expected";
				return 0;
			}
			Item = new OPERATOR_ITEM;
			Item->Operator = opLiteral;
			Item->Value = d;
		}
	}

	if (Item && negative_) {
		OPERATOR_ITEM* Root = new OPERATOR_ITEM;
		Root->Operator = opMultiply;
		Root->left_ = new OPERATOR_ITEM;
		Root->left_->Operator = opLiteral;
		Root->left_->Value = -1;
		Root->right_ = Item;
		Item = Root;
	}

	return Item;
}


// "$" Integer
GerberMacro::OPERATOR_ITEM* GerberMacro::Variable() {
	int            i;
	OPERATOR_ITEM* Item;

	SkipWhiteSpace();

	if (Index >= Length)      return 0;
	if (Buffer[Index] != '$') return 0;
	Index++;

	if (!Integer(&i)) {
		Index--;
		return 0;
	}

	Item = new OPERATOR_ITEM;
	Item->Operator = opVariable;
	Item->Index = i;

	return Item;
}


bool GerberMacro::Comment() {
	while (Index < Length) {
		if (Buffer[Index] == '*') return true;
		Index++;
	}

	return false;
}


bool GerberMacro::Circle() {
	PRIMITIVE_ITEM* Item;

	int  j;
	bool b = true;

	SkipWhiteSpace();

	const int ModifierCount = 5;

	Item = new PRIMITIVE_ITEM;
	Item->Primitive = pCircle;
	Item->ModifierCount = ModifierCount;
	Item->Modifier = new OPERATOR_ITEM * [ModifierCount];

	for (j = 0; j < ModifierCount; j++) {
		Item->Modifier[j] = 0;
	}

	SkipWhiteSpace();

	j = 0;
	while (b && Index < Length && j < ModifierCount) {
		if (Buffer[Index] == ',') {
			Index++;
			b = (Item->Modifier[j++] = Modifier());
		}
		else {
			break; // It is legal to specify fewer modifiers than the maximum
		}
		SkipWhiteSpace();
	}

	Add(Item);

	return b;
}


bool GerberMacro::Line_Vector() {
	PRIMITIVE_ITEM* Item;

	int  j;
	bool b = true;

	SkipWhiteSpace();

	const int ModifierCount = 7;

	Item = new PRIMITIVE_ITEM;
	Item->Primitive = pLineVector;
	Item->ModifierCount = ModifierCount;
	Item->Modifier = new OPERATOR_ITEM * [ModifierCount];

	for (j = 0; j < ModifierCount; j++) {
		Item->Modifier[j] = 0;
	}

	SkipWhiteSpace();

	j = 0;
	while (b && Index < Length && j < ModifierCount) {
		if (Buffer[Index] == ',') {
			Index++;
			b = (Item->Modifier[j++] = Modifier());
		}
		else {
			break;
		}
		SkipWhiteSpace();
	}

	Add(Item);

	return b;
}


bool GerberMacro::Line_Center() {
	PRIMITIVE_ITEM* Item;

	int  j;
	bool b = true;

	SkipWhiteSpace();

	const int ModifierCount = 6;

	Item = new PRIMITIVE_ITEM;
	Item->Primitive = pLineCenter;
	Item->ModifierCount = ModifierCount;
	Item->Modifier = new OPERATOR_ITEM * [ModifierCount];

	for (j = 0; j < ModifierCount; j++) {
		Item->Modifier[j] = 0;
	}

	SkipWhiteSpace();

	j = 0;
	while (b && Index < Length && j < ModifierCount) {
		if (Buffer[Index] == ',') {
			Index++;
			b = (Item->Modifier[j++] = Modifier());
		}
		else {
			break;
		}
		SkipWhiteSpace();
	}

	Add(Item);

	return b;
}


bool GerberMacro::Line_LowerLeft() {
	PRIMITIVE_ITEM* Item;

	int  j;
	bool b = true;

	SkipWhiteSpace();

	const int ModifierCount = 6;

	Item = new PRIMITIVE_ITEM;
	Item->Primitive = pLineLowerLeft;
	Item->ModifierCount = ModifierCount;
	Item->Modifier = new OPERATOR_ITEM * [ModifierCount];

	for (j = 0; j < ModifierCount; j++) {
		Item->Modifier[j] = 0;
	}

	SkipWhiteSpace();

	j = 0;
	while (b && Index < Length && j < ModifierCount) {
		if (Buffer[Index] == ',') {
			Index++;
			b = (Item->Modifier[j++] = Modifier());
		}
		else {
			break;
		}
		SkipWhiteSpace();
	}

	Add(Item);

	return b;
}


bool GerberMacro::Outline() {
	PRIMITIVE_ITEM* Item;

	int  j;
	bool b = true;

	int ModifierCount;
	int exposure_;
	int N;

	SkipWhiteSpace();

	if (Index >= Length)      return false;
	if (Buffer[Index] != ',') return false;
	Index++;

	SkipWhiteSpace();

	if (!Integer(&exposure_)) {
		LOG(ERROR) << "Error: Integer exposure expected";
		return false;
	}

	SkipWhiteSpace();

	if (Index >= Length)      return false;
	if (Buffer[Index] != ',') return false;
	Index++;

	SkipWhiteSpace();

	if (!Integer(&N)) {
		LOG(ERROR) << "Error: Integer number of outline points expected";
		return false;
	}

	ModifierCount = 5 + 2 * N;

	Item = new PRIMITIVE_ITEM;
	Item->Primitive = pOutline;
	Item->ModifierCount = ModifierCount;
	Item->Modifier = new OPERATOR_ITEM * [ModifierCount];

	for (j = 0; j < ModifierCount; j++) {
		Item->Modifier[j] = 0;
	}

	Item->Modifier[0] = new OPERATOR_ITEM;
	Item->Modifier[0]->Operator = opLiteral;
	Item->Modifier[0]->Value = exposure_;

	Item->Modifier[1] = new OPERATOR_ITEM;
	Item->Modifier[1]->Operator = opLiteral;
	Item->Modifier[1]->Value = N;

	SkipWhiteSpace();

	j = 2;
	while (b && Index < Length && j < ModifierCount) {
		if (Buffer[Index] == ',') {
			Index++;
			b = (Item->Modifier[j++] = Modifier());
		}
		else {
			break;
		}
		SkipWhiteSpace();
	}

	Add(Item);

	return b;
}


bool GerberMacro::Polygon() {
	PRIMITIVE_ITEM* Item;

	int  j;
	bool b = true;

	const int ModifierCount = 6;

	Item = new PRIMITIVE_ITEM;
	Item->Primitive = pPolygon;
	Item->ModifierCount = ModifierCount;
	Item->Modifier = new OPERATOR_ITEM * [ModifierCount];

	for (j = 0; j < ModifierCount; j++) {
		Item->Modifier[j] = 0;
	}

	SkipWhiteSpace();

	j = 0;
	while (b && Index < Length && j < ModifierCount) {
		if (Buffer[Index] == ',') {
			Index++;
			b = (Item->Modifier[j++] = Modifier());
		}
		else {
			break;
		}
		SkipWhiteSpace();
	}

	Add(Item);

	return b;
}


bool GerberMacro::Moire() {
	PRIMITIVE_ITEM* Item;

	int  j;
	bool b = true;

	const int ModifierCount = 9;

	Item = new PRIMITIVE_ITEM;
	Item->Primitive = pMoire;
	Item->ModifierCount = ModifierCount;
	Item->Modifier = new OPERATOR_ITEM * [ModifierCount];

	for (j = 0; j < ModifierCount; j++) {
		Item->Modifier[j] = 0;
	}

	SkipWhiteSpace();

	j = 0;
	while (b && Index < Length && j < ModifierCount) {
		if (Buffer[Index] == ',') {
			Index++;
			b = (Item->Modifier[j++] = Modifier());
		}
		else {
			break;
		}
		SkipWhiteSpace();
	}

	Add(Item);

	return b;
}


bool GerberMacro::Thermal() {
	PRIMITIVE_ITEM* Item;

	int  j;
	bool b = true;

	const int ModifierCount = 6;

	Item = new PRIMITIVE_ITEM;
	Item->Primitive = pThermal;
	Item->ModifierCount = ModifierCount;
	Item->Modifier = new OPERATOR_ITEM * [ModifierCount];

	for (j = 0; j < ModifierCount; j++) {
		Item->Modifier[j] = 0;
	}

	SkipWhiteSpace();

	j = 0;
	while (b && Index < Length && j < ModifierCount) {
		if (Buffer[Index] == ',') {
			Index++;
			b = (Item->Modifier[j++] = Modifier());
		}
		else {
			break;
		}
		SkipWhiteSpace();
	}

	Add(Item);

	return b;
}


bool GerberMacro::Assignment() {
	PRIMITIVE_ITEM* Item;

	int VarIndex;

	if (!Integer(&VarIndex)) return false;

	SkipWhiteSpace();

	if (Index > Length)       return false;
	if (Buffer[Index] != '=') return false;
	Index++;

	Item = new PRIMITIVE_ITEM;
	Item->Primitive = pAssignment;
	Item->ModifierCount = 1;
	Item->Modifier = new OPERATOR_ITEM * [1];
	Item->Modifier[0] = Modifier();
	Item->Index = VarIndex;

	if (!Item->Modifier[0]) {
		delete Item;
		return false;
	}

	SkipWhiteSpace();

	Add(Item);

	return true;
}


bool GerberMacro::Primitive() {
	int Identifier;

	SkipWhiteSpace();

	if (Index >= Length) return false;

	if (!Integer(&Identifier)) {
		if (Buffer[Index] == '$') {
			Index++;
			Identifier = pAssignment;
		}
		else {
			LOG(ERROR) << "Error: Macro primitive expected";
			return false;
		}
	};

	switch (Identifier) {
	case pComment:
		return Comment();

	case pCircle:
		return Circle();

	case pLineVector:
	case pLineVector2:
		return Line_Vector();

	case pLineCenter:
		return Line_Center();

	case pLineLowerLeft:
		return Line_LowerLeft();

	case pEndOfFile:
		return true;

	case pOutline:
		return Outline();

	case pPolygon:
		return Polygon();

	case pMoire:
		return Moire();

	case pThermal:
		return Thermal();

	case pAssignment:
		return Assignment();

	default:
		LOG(ERROR) << "Error: Unknown Macro Primitive: " << Buffer[Index];
		return false;
	}

	LOG(ERROR) << "Error: End of block expected";
	return false;
}


bool GerberMacro::LoadMacro(const char* buffer, unsigned Length, bool Inches) {
	GerberMacro::Buffer = buffer;
	GerberMacro::Length = Length;
	GerberMacro::Inches = Inches;
	GerberMacro::Index = 0;

	if (!Primitive())         return false;
	SkipWhiteSpace();
	if (Index >= Length)      return false;
	if (buffer[Index] != '*') return false;
	Index++;
	SkipWhiteSpace();

	while (Primitive()) {
		SkipWhiteSpace();
		if (Index >= Length)      return false;
		if (buffer[Index] != '*') return false;
		Index++;
		SkipWhiteSpace();
	}

	return Index == Length;
}

