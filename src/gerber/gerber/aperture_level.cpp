#include "aperture_level.h"
#include "gerber_level.h"

#include <cmath>
#include <glog/logging.h>


ApertureLevel::ApertureLevel(const std::vector<std::shared_ptr<RenderCommand>>& renders, const BoundBox& box)
	: render_commands_(renders), bound_box_(box)
{
}

void ApertureLevel::Draw() {
	NewPrimitime();

	struct OBJECT {
		std::vector<std::shared_ptr<RenderCommand>> commands{};
	};
	std::list<std::shared_ptr<OBJECT>> objects{};

	auto object = std::make_shared<OBJECT>();
	objects.push_front(object);
	for (const auto& render : render_commands_) {
		object->commands.push_back(render);

		if (
			render->command_ == RenderCommand::cStroke ||
			render->command_ == RenderCommand::cFill ||
			render->command_ == RenderCommand::cErase
			) {
			object = std::make_shared<OBJECT>();
			objects.push_front(object);
		}
	}

	for (const auto& object : objects) {
		for (const auto& render : object->commands) {
			switch (render->command_) {
			case RenderCommand::cRectangle:
				DrawRect(render->X, render->Y, render->W, render->H);
				break;

			case RenderCommand::cCircle:
				DrawCircle(render->X, render->Y, render->W);
				break;

			case RenderCommand::cBeginLine:
				BeginLine(render->X, render->Y);
				break;

			case RenderCommand::cLine:
				DrawLine(render->X, render->Y);
				break;

			case RenderCommand::cArc:
				DrawArc(render->X, render->Y, render->A);
				break;

			case RenderCommand::cClose:
				Close();
				break;

			case RenderCommand::cStroke:
				Stroke();
				break;

			case RenderCommand::cFill:
				Fill();
				break;

			case RenderCommand::cErase:
				Erase(bound_box_);
				break;

			default:
				LOG(ERROR) << "Error: Unrecognised Aperture Render Command " << render->command_;
				break;
			}
		}
	}
}

void ApertureLevel::Erase(const BoundBox& box) {

}

void ApertureLevel::Fill() {
	NewPrimitime();
}

void ApertureLevel::Stroke() {
	Fill();
}

void ApertureLevel::Close() {
	Fill();
}

void ApertureLevel::DrawLine(double x, double y) {
	primitive_->path_.lineTo(x * GerberLevel::kTimes, y * GerberLevel::kTimes);
}

void ApertureLevel::BeginLine(double x, double y) {
	primitive_->path_.moveTo(x * GerberLevel::kTimes, y * GerberLevel::kTimes);
}

void ApertureLevel::DrawCircle(double x, double y, double w) {
	primitive_->path_.addEllipse((x - w / 2.0) * GerberLevel::kTimes, (y - w / 2) * GerberLevel::kTimes, w * GerberLevel::kTimes, w * GerberLevel::kTimes);
}

void ApertureLevel::DrawRect(double x, double y, double w, double h) {
	x *= GerberLevel::kTimes;
	y *= GerberLevel::kTimes;
	w *= GerberLevel::kTimes;
	h *= GerberLevel::kTimes;

	primitive_->path_.addRect(x, y, w, h);
}

void ApertureLevel::NewPrimitime()
{
	if (primitive_ && !primitive_->path_.isEmpty()) {
		primitives_.emplace_back(std::move(primitive_));
	}

	primitive_ = std::make_unique<Primitive>();
}

void ApertureLevel::DrawArcScaled(double x, double y, double degree) {
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

void ApertureLevel::DrawArc(double x, double y, double degree) {
	x *= GerberLevel::kTimes;
	y *= GerberLevel::kTimes;
	DrawArcScaled(x, y, degree);
}
