#include "rectangle_aperture.h"
#include "gerber/gerber_command.h"

constexpr double kTwo = 2.0;


void RectangleAperture::RenderAperture() {
	auto render = std::make_shared<RenderCommand>(RenderCommand::cRectangle);
	render->X = -dimension_x_ / kTwo;
	render->Y = -dimension_y_ / kTwo;
	render->W = dimension_x_;
	render->H = dimension_y_;
	render_commands_.push_back(render);

	RenderHole();
	render_commands_.push_back(std::make_shared<RenderCommand>(RenderCommand::cFill));
}

RectangleAperture::RectangleAperture(int code, double w, double h, double x, double y) : Aperture(code) {
	dimension_x_ = w;
	dimension_y_ = h;

	bound_box_ = BoundBox(-w / kTwo, w / kTwo, h / kTwo, -h / kTwo);

	if (x > 0.0) {
		if (y > 0.0) {
			HoleRectangle(x, y);
		}
		else {
			HoleCircle(x);
		}
	}
}

bool RectangleAperture::SolidCircle() {
	return false;
}

bool RectangleAperture::SolidRectangle() {
	return  hole_x_ < 0.0 && hole_y_ < 0.0;
}
