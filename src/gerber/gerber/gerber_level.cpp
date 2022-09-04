#include "plotter.h"
#include "engine/qt_engine.h"
#include "aperture/aperture.h"
#include "gerber_level.h"
#include "aperture_level.h"
#include <glog/logging.h>

#include <iostream>
#include <QPainter>

constexpr double kPi = 3.141592653589793238463;
constexpr double kVeryLarge = 1e5;
constexpr double kVeryLittle = -1e5;


void StrokesToFillsConverter::Segment::Add(const std::shared_ptr<RenderCommand>& command) {
	closed_ = false;
	command_list_.push_back(command);
}

bool StrokesToFillsConverter::Segment::IsClosed() {
	if (closed_) {
		return true;
	}

	if (command_list_.empty()) {
		return false;
	}

	if (command_list_.back()->end_.first == command_list_.front()->X &&
		command_list_.back()->end_.second == command_list_.front()->Y) {
		closed_ = true;
		return true;
	}

	return false;
}

void StrokesToFillsConverter::Segment::Reverse() {
	if (IsClosed()) {
		return; // Don't bother reversing closed loops
	}

	if (command_list_.empty()) {
		return;
	}

	auto old_list = command_list_;
	command_list_.clear();

	auto begin_line = old_list.front();

	double x = begin_line->X;
	double y = begin_line->Y;

	auto begin = old_list.begin();
	for (auto iter = ++begin; iter != old_list.end(); ++iter) {
		auto command = *iter;

		switch (command->command_) {
		case RenderCommand::cLine:
			command->X = x;
			command->Y = y;
			std::swap(x, command->end_.first);
			std::swap(y, command->end_.second);
			command_list_.push_front(command);
			break;

		case RenderCommand::cArc:
			command->A *= -1;
			std::swap(x, command->end_.first);
			std::swap(y, command->end_.second);
			command_list_.push_front(command);
			break;

		default:
			break;
		}
	}

	begin_line->X = x;
	begin_line->Y = y;
	command_list_.push_front(begin_line);
}

void StrokesToFillsConverter::Segment::Isolate() {
	if (prev_) {
		prev_->next_ = next_;
	}

	if (next_) {
		next_->prev_ = prev_;
	}

	next_ = prev_ = nullptr;
}

void StrokesToFillsConverter::NewSegment() {
	auto tmp = std::make_shared<Segment>();

	if (last_segment_ != nullptr) {
		last_segment_->next_ = tmp;
	}
	else {
		segment_list_ = tmp;
	}

	tmp->prev_ = last_segment_;
	last_segment_ = tmp;
}

void StrokesToFillsConverter::ExtractSegments() {
	auto old_list = level_.render_commands_;
	level_.render_commands_.clear();

	bool is_outline = false;
	for (const auto& render : old_list) {
		switch (render->command_) {
		case RenderCommand::cBeginLine:
			if (is_outline) {
				level_.Add(render);
			}
			else {
				NewSegment();
				last_segment_->Add(render);
			}
			break;

		case RenderCommand::cLine:
			if (is_outline) {
				level_.Add(render);
			}
			else {
				last_segment_->Add(render);
			}
			break;

		case RenderCommand::cArc:
			if (is_outline) {
				level_.Add(render);
			}
			else {
				last_segment_->Add(render);
			}
			break;

		case RenderCommand::cStroke:
			break;

		case RenderCommand::cClose:
		case RenderCommand::cFill:
			level_.Add(render);
			break;

		case RenderCommand::cBeginOutline:
			is_outline = true;
			level_.Add(render);
			break;

		case RenderCommand::cEndOutline:
			is_outline = false;
			level_.Add(render);
			break;

		case RenderCommand::cApertureSelect:
		case RenderCommand::cFlash:
			break;

		case RenderCommand::cRectangle:
		case RenderCommand::cCircle:
		case RenderCommand::cErase:
		default:
			break;
		}
	}
}

std::shared_ptr<StrokesToFillsConverter::Segment> StrokesToFillsConverter::FindNeighbour(const std::shared_ptr<Segment>& current) {
	if (current->command_list_.empty() || current->IsClosed()) {
		return nullptr;
	}

	auto candidate = segment_list_;
	while (candidate) {
		if (candidate != current && !candidate->command_list_.empty() && !candidate->IsClosed()) {
			if (current->command_list_.back()->end_.first == candidate->command_list_.front()->X &&
				current->command_list_.back()->end_.second == candidate->command_list_.front()->Y) {
				return candidate;
			}

			if (current->command_list_.back()->end_.first == candidate->command_list_.back()->end_.first &&
				current->command_list_.back()->end_.second == candidate->command_list_.back()->end_.second) {
				candidate->Reverse();
				return candidate;
			}
		}

		candidate = candidate->next_;
	}

	// No candidates found, but run the test again checking for near segments.
	// Points are considered the same if they are closer than 1 μm.
	// This is required because many Gerber generators make rounding errors.

	candidate = segment_list_;
	double dX = 0.0;
	double dY = 0.0;

	while (candidate != nullptr) {
		if (candidate != current && !candidate->command_list_.empty() && !candidate->IsClosed()) {
			dX = fabs(current->command_list_.back()->end_.first - candidate->command_list_.front()->X);
			dY = fabs(current->command_list_.back()->end_.second - candidate->command_list_.front()->Y);
			if (dX < 1e-3 && dY < 1e-3) {// NOLINT
				LOG(WARNING) << "Strokes2Fills - Warning: Joining segments that are close, but not coincident:";
				return candidate;
			}
			dX = fabs(current->command_list_.back()->end_.first - candidate->command_list_.back()->end_.first);
			dY = fabs(current->command_list_.back()->end_.second - candidate->command_list_.back()->end_.second);
			if (dX < 1e-3 && dY < 1e-3) {// NOLINT
				candidate->Reverse();
				std::cout << "Strokes2Fills - Warning: Joining segments that are close, but not coincident:" << std::endl;
				std::cout << "dX = " << dX << " mm (" << dX / 25.4e-3 << " mil)" << std::endl;// NOLINT
				std::cout << "dY = " << dY << " mm (" << dY / 25.4e-3 << " mil)" << std::endl;// NOLINT
				return candidate;
			}
		}
		candidate = candidate->next_;
	}
	return nullptr;
}

void StrokesToFillsConverter::JoinSegments() {
	auto current = segment_list_;
	while (current) {
		auto neighbour = FindNeighbour(current);

		if (neighbour) {
			if (segment_list_ == neighbour) {
				segment_list_ = segment_list_->next_;
			}

			if (last_segment_ == neighbour) {
				last_segment_ = last_segment_->prev_;
			}

			neighbour->Isolate();

			auto begin_line = neighbour->command_list_.front();
			neighbour->command_list_.pop_front();

			for (const auto& each : neighbour->command_list_) {
				current->command_list_.push_back(each);
			}
		}
		else {
			current = current->next_;
		}
	}
}

void StrokesToFillsConverter::AddSegments() {
	if (segment_list_ == nullptr) {
		return;
	}

	level_.Add(std::make_shared<BeginOutlineCommand>());

	while (segment_list_ != nullptr) {
		for (const auto& each : segment_list_->command_list_) {
			level_.render_commands_.push_back(each);
		}

		if (!segment_list_->IsClosed()) {
			level_.Add(std::make_shared<CloseCommand>());
		}

		auto tmp = segment_list_;
		segment_list_ = segment_list_->next_;
		tmp->Isolate();
	}
	last_segment_ = nullptr;

	level_.Add(std::make_shared<FillCommand>());
	level_.Add(std::make_shared<EndOutlineCommand>());
}

StrokesToFillsConverter::StrokesToFillsConverter(GerberLevel& level) : level_(level)
{
}




GerberLevel::GerberLevel(const std::shared_ptr<GerberLevel>& previous_level, UnitType::Type unit) :
	bound_box_(kVeryLarge, kVeryLittle, kVeryLittle, kVeryLarge) {

	count_x_ = count_y_ = 1;
	step_x_ = step_y_ = 0.0;

	negative_ = false;
	unit_ = unit;

	if (previous_level) {
		SetName(previous_level->name_);

		count_x_ = previous_level->count_x_;
		count_y_ = previous_level->count_y_;
		step_x_ = previous_level->step_x_;
		step_y_ = previous_level->step_y_;

		negative_ = previous_level->negative_;
	}
}

GerberLevel::~GerberLevel() {

}

bool GerberLevel::IsCopyLayer() const
{
	return count_x_ > 1 || count_y_ > 1;
}

double GerberLevel::GetRight() const
{
	if (count_x_ > 1) {
		return bound_box_.Left() + step_x_ * (count_x_ - 1) + bound_box_.Right() - bound_box_.Left();
	}

	return bound_box_.Right();
}

double GerberLevel::GetTop() const
{
	if (count_y_ > 1) {
		return bound_box_.Bottom() + step_y_ * (count_y_ - 1) + bound_box_.Top() - bound_box_.Bottom();
	}

	return bound_box_.Top();
}

UnitType::Type GerberLevel::GetUnit() const
{
	return unit_;
}

void GerberLevel::SetUnit(UnitType::Type type)
{
	unit_ = type;
}

std::string GerberLevel::GetName() const
{
	return name_;
}

void GerberLevel::SetName(const std::string& name) {
	name_ = name;
}

bool GerberLevel::IsNegative() const
{
	return negative_;
}

void GerberLevel::SetNegative(bool negative)
{
	negative_ = negative;
}

BoundBox GerberLevel::GetBoundBox() const
{
	return bound_box_;
}

void GerberLevel::Add(const std::shared_ptr<RenderCommand>& command) {
	render_commands_.push_back(command);
}

std::vector<std::shared_ptr<RenderCommand>> GerberLevel::RenderCommands() const {
	return render_commands_;
}

void GerberLevel::ConvertStrokesToFills() {
	StrokesToFillsConverter converter(*this);
	converter.ExtractSegments();
	converter.JoinSegments();
	converter.AddSegments();
}

int GerberLevel::Render(QtEngine* engine) {
	engine_ = engine;

	if (engine_->convert_strokes2fills_) {
		ConvertStrokesToFills();
	}

	auto renders = RenderCommands();
	for (const auto& render : renders) {
		if (auto ret = Draw(render)) {
			return ret;
		}
	}

	return 0;
}

int GerberLevel::Draw(const std::shared_ptr<RenderCommand>& render)
{
	switch (render->command_) {
	case RenderCommand::cRectangle:
		DrawRectangle(render->X, render->Y, render->W, render->H);
		break;

	case RenderCommand::cCircle:
		DrawCircle(render->X, render->Y, render->W / 2.0); // NOLINT
		break;

	case RenderCommand::cBeginLine:
		if (engine_->outline_path_) {
			BeginLine(render->X, render->Y);
		}
		else if (engine_->aperture_img_.aperture_->SolidCircle()) {
			BeginSolidCircleLine(render->X, render->Y);
		}
		else if (engine_->aperture_img_.aperture_->SolidRectangle()) {
			engine_->rect_x_ = render->X;
			engine_->rect_y_ = render->Y;
		}
		else {
			LOG(ERROR) << "Error: Only solid circular or rectangular apertures can be used for paths";
			return 1;
		}
		break;

	case RenderCommand::cLine:
		if (engine_->outline_path_ || engine_->aperture_img_.aperture_->SolidCircle()) {
			DrawLine(render->X, render->Y);
		}
		else if (engine_->aperture_img_.aperture_->SolidRectangle()) {
			DrawRectLine(
				engine_->rect_x_, engine_->rect_y_,
				render->X, render->Y,
				engine_->aperture_img_.aperture_->bound_box_.Width(), engine_->aperture_img_.aperture_->bound_box_.Height()
			);
			engine_->rect_x_ = render->X;
			engine_->rect_y_ = render->Y;
		}
		else {
			LOG(ERROR) << "Error: Only solid circular or rectangular apertures can be used for paths";
			return 2;
		}
		break;

	case RenderCommand::cArc:
		if (engine_->outline_path_ || engine_->aperture_img_.aperture_->SolidCircle()) {
			DrawArc(render->X, render->Y, render->A);
		}
		else {
			LOG(ERROR) << "Error: Only solid circular apertures can be used for arcs";
			return 3;
		}
		break;

	case RenderCommand::cFlash:
		if (auto ret = Flash(render->X, render->Y)) {
			return ret;
		}

		break;

	case RenderCommand::cClose:
		Close();
		break;

	case RenderCommand::cStroke:
		Stroke();
		break;

	case RenderCommand::cFill:
		FillEvenOdd();
		break;

	case RenderCommand::cBeginOutline:
		BeginOutline();
		break;

	case RenderCommand::cEndOutline:
		EndOutline();
		break;

	case RenderCommand::cApertureSelect: {
		auto aperture = render->aperture_;
		if (!aperture) {
			LOG(ERROR) << "Error: Null Aperture";
			return 5; // NOLINT
		}

		if (!PrepareExistAperture(aperture->Code())) {
			NewAperture(aperture);
			ApertureLevel aperture_level(aperture->Render(), aperture->bound_box_);
			aperture_level.Draw();
			EndDrawNewAperture(aperture->Code(), aperture_level.primitives_);
		}

		break;
	}
	default:
		break;
	}
	return {};
}

void GerberLevel::BeginSolidCircleLine(double x, double y) {
	x *= kTimes;
	y *= kTimes;

	if (negative_) {
		auto pen = engine_->current_painter_->pen();
		pen.setColor(QColor(255, 255, 255));
		pen.setWidthF(engine_->aperture_img_.aperture_->bound_box_.Width() * kTimes);
		engine_->current_painter_->setPen(pen);
	}
	else {
		auto pen = engine_->current_painter_->pen();
		pen.setColor(QColor(engine_->count_ % 256, (engine_->count_ + 53) % 256, (engine_->count_ + 125) % 256, 200));
		pen.setWidthF(engine_->aperture_img_.aperture_->bound_box_.Width() * kTimes);
		engine_->current_painter_->setPen(pen);
	}

	primitive_->path_.moveTo(x, y);
}

void GerberLevel::EndDrawNewAperture(int code, const std::vector<std::unique_ptr<Primitive>>& primitives) {
	engine_->aperture_painter_->setPen(QPen(QColor(0, 0, 0, 0), 1.0));

	if (negative_) {
		engine_->aperture_painter_->setBrush(QColor(255, 255, 255));
	}
	else {
		engine_->aperture_painter_->setBrush(QColor(engine_->count_ % 256, (engine_->count_ + 53) % 256, (engine_->count_ + 125) % 256, 200));
	}

	for (const auto& each : primitives) {
		engine_->aperture_painter_->drawPath(each->GetPath());
	}

	engine_->AddAperture(code);
	engine_->aperture_painter_ = nullptr;
}

int GerberLevel::Flash(double x, double y) {
    NewPrimitive(Primitive::kNormal, 1.0);
    NewPrimitive(QRectF(
                        x * kTimes + engine_->aperture_img_.aperture_->bound_box_.Left()* kTimes,
                        y * kTimes + engine_->aperture_img_.aperture_->bound_box_.Bottom() * kTimes,
                        engine_->aperture_img_.aperture_->bound_box_.Width() * kTimes,
                        engine_->aperture_img_.aperture_->bound_box_.Height() * kTimes
                ),
                *engine_->aperture_img_.pixmap_,
                QRectF(0.0, 0.0, engine_->aperture_img_.pixmap_->width(), engine_->aperture_img_.pixmap_->height())
    );
    
	return 0;
}

void GerberLevel::BeginOutline() {
	NewPrimitive(Primitive::kNormal, 1.0);
	engine_->outline_path_ = true;
}

void GerberLevel::EndOutline() {
	NewPrimitive(Primitive::kNormal, 1.0);
	engine_->outline_path_ = false;
}

void GerberLevel::FillEvenOdd() {
}

void GerberLevel::Stroke() {
	NewPrimitive(Primitive::kStroke, engine_->aperture_img_.aperture_->bound_box_.Width() * kTimes);
}

void GerberLevel::Close() {}

void GerberLevel::DrawArcScaled(double x, double y, double degree) {
	if (fabs(primitive_->path_.currentPosition().x() - x) < 1e-15 && fabs(primitive_->path_.currentPosition().y() - y) < 1e-15) {
		return;
	}

	if (fabs(degree) > 45.001) { // Slightly large so that the circle has 4 segments
		degree /= 2.0;
		DrawArcScaled(x, y, degree);
		DrawArcScaled(x, y, degree);
		return;
	}

	const auto delta_x = primitive_->path_.currentPosition().x() - x;
	const auto delta_y = primitive_->path_.currentPosition().y() - y;
	double radius = sqrt(delta_y * delta_y + delta_x * delta_x);

	double t, t1, t2;
	double e, de, dt, rb, xb, yb;

	constexpr double kPi = 3.141592653589793;
	double b = degree * kPi / 180 / 2.0;

	double a1 = atan2(delta_y, delta_x);

	double a4 = a1 + degree * kPi / 180;
	double x4 = radius * cos(a4);
	double y4 = radius * sin(a4);

	double a5 = a1 + b;
	double x5 = radius * cos(a5);
	double y5 = radius * sin(a5);

	b = cos(b);
	double x6 = x5 / b;
	double y6 = y5 / b;

	// Best extimate
	t1 = 6 * x6 - 3 * delta_x - 3 * (x4);
	t2 = 6 * y6 - 3 * delta_y - 3 * (y4);
	if (fabs(t1) > fabs(t2)) {
		t = (8 * x5 - 4 * delta_x - 4 * (x4)) / t1;
	}
	else {
		t = (8 * y5 - 4 * delta_y - 4 * (y4)) / t2;
	}

	// Newton-Raphson to fit B(1/3) to the circle
	radius = radius * radius;

	dt = 1e-3;
	t2 = 1.0 / 3.0; // Fitting B(1/3) to the circle is close enough to the
	t1 = 1.0 - t2;  // optimum solution.

	double x2 = (1.0 - t) * delta_x + t * x6;
	double y2 = (1.0 - t) * delta_y + t * y6;
	double x3 = (1.0 - t) * (x4)+t * x6;
	double y3 = (1.0 - t) * (y4)+t * y6;
	xb = t1 * t1 * t1 * delta_x + 3.0 * t1 * t1 * t2 * (x2)+3.0 * t1 * t2 * t2 * (x3)+t2 * t2 * t2 * (x4);
	yb = t1 * t1 * t1 * delta_y + 3.0 * t1 * t1 * t2 * (y2)+3.0 * t1 * t2 * t2 * (y3)+t2 * t2 * t2 * (y4);
	rb = xb * xb + yb * yb;
	e = rb - radius;
	while (e > 1e-12) {
		x2 = (1.0 - (t + dt)) * delta_x + (t + dt) * x6;
		y2 = (1.0 - (t + dt)) * delta_y + (t + dt) * y6;
		x3 = (1.0 - (t + dt)) * (x4)+(t + dt) * x6;
		y3 = (1.0 - (t + dt)) * (y4)+(t + dt) * y6;
		xb = t1 * t1 * t1 * delta_x + 3.0 * t1 * t1 * t2 * (x2)+3.0 * t1 * t2 * t2 * (x3)+t2 * t2 * t2 * (x4);
		yb = t1 * t1 * t1 * delta_y + 3.0 * t1 * t1 * t2 * (y2)+3.0 * t1 * t2 * t2 * (y3)+t2 * t2 * t2 * (y4);
		rb = xb * xb + yb * yb;
		de = (rb - radius - e) / dt;

		t -= e / de; // Newton-Raphson

		x2 = (1.0 - t) * delta_x + t * x6;
		y2 = (1.0 - t) * delta_y + t * y6;
		x3 = (1.0 - t) * (x4)+t * x6;
		y3 = (1.0 - t) * (y4)+t * y6;
		xb = t1 * t1 * t1 * delta_x + 3.0 * t1 * t1 * t2 * (x2)+3.0 * t1 * t2 * t2 * (x3)+t2 * t2 * t2 * (x4);
		yb = t1 * t1 * t1 * delta_y + 3.0 * t1 * t1 * t2 * (y2)+3.0 * t1 * t2 * t2 * (y3)+t2 * t2 * t2 * (y4);
		rb = xb * xb + yb * yb;
		e = rb - radius;
	}

	primitive_->path_.cubicTo(x2 + x, y2 + y, x3 + x, y3 + y, x4 + x, y4 + y);
}

void GerberLevel::DrawArc(double x, double y, double degree) {
	x *= kTimes;
	y *= kTimes;
	DrawArcScaled(x, y, degree);
}

void GerberLevel::DrawLine(double x, double y) {
	x *= kTimes;
	y *= kTimes;

	primitive_->path_.lineTo(x, y);
}

void GerberLevel::BeginLine(double x, double y) {
	x *= kTimes;
	y *= kTimes;

	primitive_->path_.moveTo(x, y);
}

void GerberLevel::DrawCircle(double x, double y, double r) {
	x *= kTimes;
	y *= kTimes;
	r *= kTimes;

	primitive_->path_.addEllipse(x - r, y - r, r * 2, r * 2);
}

void GerberLevel::DrawRectangle(double x, double y, double w, double h) {
	x *= kTimes;
	y *= kTimes;
	w *= kTimes;
	h *= kTimes;

	primitive_->path_.addRect(x, y, w, h);
}

void GerberLevel::DrawRectLine(double x1, double y1, double x2, double y2, double w, double h) {
	x1 *= kTimes;
	y1 *= kTimes;
	x2 *= kTimes;
	y2 *= kTimes;
	w *= kTimes;
	h *= kTimes;

	w /= 2.0;
	h /= 2.0;

	if (x2 > x1) {
		if (y2 > y1) {
			primitive_->path_.moveTo(x1 - w, y1 - h);
			primitive_->path_.lineTo(x1 + w, y1 - h);
			primitive_->path_.lineTo(x2 + w, y2 - h);
			primitive_->path_.lineTo(x2 + w, y2 + h);
			primitive_->path_.lineTo(x2 - w, y2 + h);
			primitive_->path_.lineTo(x1 - w, y1 + h);
		}
		else if (y1 > y2) {
			primitive_->path_.moveTo(x1 - w, y1 - h);
			primitive_->path_.lineTo(x2 - w, y2 - h);
			primitive_->path_.lineTo(x2 + w, y2 - h);
			primitive_->path_.lineTo(x2 + w, y2 + h);
			primitive_->path_.lineTo(x1 + w, y1 + h);
			primitive_->path_.lineTo(x1 - w, y1 + h);
		}
		else { // y1 = y2
			primitive_->path_.moveTo(x1 - w, y1 - h);
			primitive_->path_.lineTo(x2 + w, y2 - h);
			primitive_->path_.lineTo(x2 + w, y2 + h);
			primitive_->path_.lineTo(x1 - w, y1 + h);
		}

	}
	else if (x1 > x2) {
		if (y2 > y1) {
			primitive_->path_.moveTo(x2 - w, y2 - h);
			primitive_->path_.lineTo(x1 - w, y1 - h);
			primitive_->path_.lineTo(x1 + w, y1 - h);
			primitive_->path_.lineTo(x1 + w, y1 + h);
			primitive_->path_.lineTo(x2 + w, y2 + h);
			primitive_->path_.lineTo(x2 - w, y2 + h);

		}
		else if (y1 > y2) {
			primitive_->path_.moveTo(x2 - w, y2 - h);
			primitive_->path_.lineTo(x2 + w, y2 - h);
			primitive_->path_.lineTo(x1 + w, y1 - h);
			primitive_->path_.lineTo(x1 + w, y1 + h);
			primitive_->path_.lineTo(x1 - w, y1 + h);
			primitive_->path_.lineTo(x2 - w, y2 + h);

		}
		else { // y1 = y2
			primitive_->path_.moveTo(x2 - w, y2 - h);
			primitive_->path_.lineTo(x1 + w, y1 - h);
			primitive_->path_.lineTo(x1 + w, y1 + h);
			primitive_->path_.lineTo(x2 - w, y2 + h);
		}

	}
	else { // x1 = x2
		if (y2 > y1) {
			primitive_->path_.moveTo(x1 - w, y1 - h);
			primitive_->path_.lineTo(x1 + w, y1 - h);
			primitive_->path_.lineTo(x2 + w, y2 + h);
			primitive_->path_.lineTo(x2 - w, y2 + h);

		}
		else { // y1 >= y2
			primitive_->path_.moveTo(x2 - w, y2 - h);
			primitive_->path_.lineTo(x2 + w, y2 - h);
			primitive_->path_.lineTo(x1 + w, y1 + h);
			primitive_->path_.lineTo(x1 - w, y1 + h);
		}
	}

	NewPrimitive(Primitive::kRectLine, engine_->aperture_img_.aperture_->bound_box_.Width() * kTimes);
}

void GerberLevel::NewPrimitive(Primitive::Type type, double line_width)
{
	if (primitive_ && !primitive_->path_.isEmpty()) {
		primitive_->type_ = type;
		primitive_->line_width_ = line_width;
		primitives_.emplace_back(std::move(primitive_));
	}

	primitive_ = std::make_unique<Primitive>();
}

void GerberLevel::NewPrimitive(const QRectF& dst, const QPixmap& pic, const QRectF& src){
    if (primitive_) {
        primitive_->type_ = Primitive::kApertureMacro;
        primitive_->dst_ = dst;
        primitive_->src_ = src;
        primitive_->pic_ = pic;
        primitives_.emplace_back(std::move(primitive_));
    }

    primitive_ = std::make_unique<Primitive>();
}

std::vector<std::shared_ptr<Primitive>> GerberLevel::GetPrimitives() const
{
	return primitives_;
}

bool GerberLevel::PrepareExistAperture(int code) {
	if (engine_->aperture_imgs_.find(code) != engine_->aperture_imgs_.end()) {
		engine_->aperture_img_ = engine_->aperture_imgs_[code];
		return true;
	}

	return false;
}

void GerberLevel::NewAperture(std::shared_ptr<Aperture> aperture) {
	const auto left = aperture->bound_box_.Left() * kTimes;
	const auto right = aperture->bound_box_.Right() * kTimes;
	const auto top = aperture->bound_box_.Top() * kTimes;
	const auto bottom = aperture->bound_box_.Bottom() * kTimes;

	auto width = engine_->trans_.TranslateLogicCoord(right - left);
	auto height = engine_->trans_.TranslateLogicCoord(top - bottom);
	if (width < 1) {
		width = 1;
	}

	if (height < 1) {
		height = 1;
	}

	engine_->aperture_img_ = QtEngine::ApertureImg(aperture, std::make_shared<QPixmap>(width * 10, height * 10));
	engine_->aperture_img_.pixmap_->fill(QColor(255, 255, 255, 0));
	engine_->aperture_painter_ = std::make_shared<QPainter>(engine_->aperture_img_.pixmap_.get());
	engine_->aperture_painter_->setWindow(left, bottom, right - left, top - bottom);
}
