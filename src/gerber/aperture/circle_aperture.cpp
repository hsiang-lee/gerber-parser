#include "circle_aperture.h"
#include "gerber/gerber_command.h"

constexpr double kTwo = 2.0;

CircleAperture::CircleAperture(int code, double d, double x, double y) : Aperture(code) {
	dimension_x_ = d;

	bound_box_ = BoundBox(-d / kTwo, d / kTwo, d / kTwo, -d / kTwo);

	if (x > 0.0) {
		if (y > 0.0) {
			HoleRectangle(x, y);
		}
		else {
			HoleCircle(x);
		}
	}
}

void CircleAperture::RenderAperture() {
	auto render = std::make_shared<RenderCommand>(RenderCommand::cCircle);
	render->X = 0.0;
	render->Y = 0.0;
	render->W = dimension_x_;
	render_commands_.push_back(render);

	RenderHole();
	render_commands_.push_back(std::make_shared<RenderCommand>(RenderCommand::cFill));
}

bool CircleAperture::SolidCircle() {
	return hole_x_ < 0.0 && hole_y_ < 0.0;
}

bool CircleAperture::SolidRectangle() {
	return false;
}
