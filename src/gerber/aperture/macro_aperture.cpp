#include "macro_aperture.h"
#include "gerber/gerber_macro.h"
#include "gerber/gerber_command.h"

constexpr double kPi = 3.141592653589793238463;
constexpr double kTwo = 2.0;

MacroAperture::MacroAperture(
	int code,
	const std::shared_ptr<GerberMacro>& macro,
	const std::vector<double>& modifier
) : Aperture(code) {
	double x = 0.0;
	double y = 0.0;
	double l = 0.0;
	double b = 0.0;
	double r = 0.0;
	double t = 0.0;
	double c = 0.0;
	double d = 0.0;
	double e = 0.0; // Temporary variables

	render_commands_ = macro->Render(modifier);

	for (auto& render : render_commands_) {
		switch (render->command_) {
		case RenderCommand::cRectangle:
			l = render->X;
			b = render->Y;
			r = render->X + render->W;
			t = render->Y + render->H;
			x = l;
			y = b;
			break;

		case RenderCommand::cCircle:
			l = render->X - render->W / kTwo;
			b = render->Y - render->W / kTwo;
			r = render->X + render->W / kTwo;
			t = render->Y + render->W / kTwo;
			x = r;
			y = 0.0;
			break;

		case RenderCommand::cBeginLine:
			l = render->X;
			b = render->Y;
			r = render->X;
			t = render->Y;
			x = l;
			y = b;
			break;

		case RenderCommand::cLine:
			l = render->X;
			b = render->Y;
			r = render->X;
			t = render->Y;
			x = l;
			y = b;
			break;

		case RenderCommand::cArc:
			c = x - render->X;   // Start relative to center
			d = y - render->Y;   // End relative to center
			e = sqrt(c * c + d * d); // Radius
			c = atan2(d, c) + render->A * kPi / 180.0; // NOLINT

			// Play safe and assume it is a full circle
			l = render->X - e;
			b = render->Y - e;
			r = render->X + e;
			t = render->Y + e;

			x = render->X + e * cos(c);
			y = render->Y + e * sin(c);
			break;

		default:
			l = r = x;
			b = t = y;
			break;
		}

		if (render == render_commands_.front()) {
			bound_box_ = BoundBox(l, r, t, b);
		}
		else {
			bound_box_.UpdateBox(l, r, t, b);
		}
	}
}

void MacroAperture::RenderAperture()
{
}

bool MacroAperture::SolidCircle() {
	return false;
}

bool MacroAperture::SolidRectangle() {
	return false;
}
