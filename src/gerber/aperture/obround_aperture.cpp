#include "obround_aperture.h"
#include "gerber/gerber_command.h"

constexpr double kTwo = 2.0;


ObroundAperture::ObroundAperture(int code, double w, double h, double x, double y) : Aperture(code) {
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

void ObroundAperture::RenderAperture() {
	if (dimension_x_ > dimension_y_) {
		auto r = dimension_y_ / kTwo;
		auto t = dimension_x_ / kTwo - r;

		render_commands_.push_back(std::make_shared<BeginLineCommand>(t, -r));
		render_commands_.push_back(std::make_shared<ArcCommand>(t, 0.0, 180.0));
		render_commands_.push_back(std::make_shared<LineCommand>(-t, r));
		render_commands_.push_back(std::make_shared<ArcCommand>(-t, 0.0, 180.0));
	}
	else {
		auto r = dimension_x_ / kTwo;
		auto t = dimension_y_ / kTwo - r;

		render_commands_.push_back(std::make_shared<BeginLineCommand>(r, t));
		render_commands_.push_back(std::make_shared<ArcCommand>(0.0, t, 180.0));
		render_commands_.push_back(std::make_shared<LineCommand>(-r, -t));
		render_commands_.push_back(std::make_shared<ArcCommand>(0.0, -t, 180.0));
	}

	render_commands_.push_back(std::make_shared<CloseCommand>());
	RenderHole();
	render_commands_.push_back(std::make_shared<FillCommand>());
}

bool ObroundAperture::SolidCircle() {
	return false;
}

bool ObroundAperture::SolidRectangle() {
	return false;
}
