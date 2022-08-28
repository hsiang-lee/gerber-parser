#include "aperture.h"
#include "gerber/gerber_command.h"


constexpr double  kPi = 3.141592653589793238463;
constexpr double kVeryLarge = 1e5;
constexpr double kVeryLittle = -1e5;
constexpr double kTwo = 2.0;


Aperture::Aperture(int code) {
	code_ = code;

	dimension_x_ = -1.0;
	dimension_y_ = -1.0;
	hole_x_ = -1.0;
	hole_y_ = -1.0;

	bound_box_ = BoundBox(kVeryLarge, kVeryLittle, kVeryLittle, kVeryLarge);

	rotation_ = 0.0;
}

Aperture::~Aperture() {// NOLINT
}

void Aperture::HoleCircle(double d) {
	hole_x_ = d;
	hole_y_ = -1.0;
}

void Aperture::HoleRectangle(double w, double h) {
	hole_x_ = w;
	hole_y_ = h;
}

void Aperture::RenderHole() {
	if (hole_x_ > 0.0) {
		if (hole_y_ < 0.0) {
			render_commands_.push_back(std::make_shared<BeginLineCommand>(hole_x_ / kTwo, 0.0));
			render_commands_.push_back(std::make_shared<ArcCommand>(0.0, 0.0, -360.0));
		}
		else {
			render_commands_.push_back(std::make_shared<BeginLineCommand>(hole_x_ / kTwo, -hole_y_ / kTwo));
			render_commands_.push_back(std::make_shared<LineCommand>(-hole_x_ / kTwo, -hole_y_ / kTwo));
			render_commands_.push_back(std::make_shared<LineCommand>(-hole_x_ / kTwo, hole_y_ / kTwo));
			render_commands_.push_back(std::make_shared<LineCommand>(hole_x_ / kTwo, hole_y_ / kTwo));
			render_commands_.push_back(std::make_shared<RenderCommand>(RenderCommand::cClose));
		}
	}
}

std::vector<std::shared_ptr<RenderCommand>> Aperture::Render() {
	if (render_commands_.empty()) {
		RenderAperture();
	}

	return render_commands_;
}
