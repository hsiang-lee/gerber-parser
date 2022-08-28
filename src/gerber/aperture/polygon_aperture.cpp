#include "polygon_aperture.h"
#include "gerber/gerber_command.h"

constexpr double kPi = 3.141592653589793238463;
constexpr double kTwo = 2.0;

PolygonAperture::PolygonAperture(int code, double w, int n, double a, double x, double y) : Aperture(code) {
	dimension_x_ = w;
	side_count_ = n;
	rotation_ = a;
	side_count_ = 0;

	bound_box_ = BoundBox(-w / kTwo, w / kTwo, w / kTwo, -w / kTwo);

	if (x > 0.0) {
		if (y > 0.0) {
			HoleRectangle(x, y);
		}
		else {
			HoleCircle(x);
		}
	}
}

void PolygonAperture::RenderAperture() {
	// Get Rotation in range [0; 360)
	while (rotation_ < 0.0) {
		rotation_ += 360;// NOLINT
	}

	while (rotation_ >= 360.0) {// NOLINT
		rotation_ -= 360;// NOLINT
	}

	auto r = dimension_x_ / kTwo;
	auto da = kTwo * kPi / side_count_;
	auto rot = rotation_ * kPi / 180.0;// NOLINT
	auto lim = kTwo * kPi - da / kTwo;

	auto render = std::make_shared<RenderCommand>(RenderCommand::cBeginLine);
	render->X = r * cos(rot);
	render->Y = r * sin(rot);
	render_commands_.push_back(render);

	for (double a = da; a < lim; a += da) {
		render = std::make_shared<RenderCommand>(RenderCommand::cLine);
		render->X = r * cos(a + rot);
		render->Y = r * sin(a + rot);
		render_commands_.push_back(render);
	}

	render_commands_.push_back(std::make_shared<RenderCommand>(RenderCommand::cClose));
	RenderHole();
	render_commands_.push_back(std::make_shared<RenderCommand>(RenderCommand::cFill));
}

bool PolygonAperture::SolidCircle() {
	return false;
}

bool PolygonAperture::SolidRectangle() {
	return false;
}
