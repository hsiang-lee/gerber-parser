#include "gerber_aperture.h"

constexpr double  kPi = 3.141592653589793238463;


GerberAperture::GerberAperture() {
	code_ = 0;
	type_ = tCircle;

	dimension_x_ = -1.0;
	dimension_y_ = -1.0;
	hole_x_ = -1.0;
	hole_y_ = -1.0;

	left_ = 1e3;
	bottom_ = 1e3;
	right_ = -1e3;
	top_ = -1e3;

	side_count_ = 0;
	rotation_ = 0.0;
}

GerberAperture::~GerberAperture() {
}

void GerberAperture::Circle(double d) {
	type_ = tCircle;
	dimension_x_ = d;

	left_ = -d / 2.0;
	bottom_ = -d / 2.0;
	right_ = d / 2.0;
	top_ = d / 2.0;
}

void GerberAperture::Rectangle(double w, double h) {
	type_ = tRectangle;
	dimension_x_ = w;
	dimension_y_ = h;

	left_ = -w / 2.0;
	bottom_ = -h / 2.0;
	right_ = w / 2.0;
	top_ = h / 2.0;
}

void GerberAperture::Obround(double w, double h) {
	type_ = tObround;
	dimension_x_ = w;
	dimension_y_ = h;

	left_ = -w / 2.0;
	bottom_ = -h / 2.0;
	right_ = w / 2.0;
	top_ = h / 2.0;
}

void GerberAperture::Polygon(double w, int n, double a) {
	type_ = tPolygon;
	dimension_x_ = w;
	side_count_ = n;
	rotation_ = a;

	left_ = -w / 2.0;
	bottom_ = -w / 2.0;
	right_ = w / 2.0;
	top_ = w / 2.0;
}

void GerberAperture::HoleCircle(double d) {
	hole_x_ = d;
	hole_y_ = -1.0;
}

void GerberAperture::HoleRectangle(double w, double h) {
	hole_x_ = w;
	hole_y_ = h;
}

void GerberAperture::UseMacro(
	std::shared_ptr<GerberMacro> macro,
	double* modifiers,
	int modifier_count
) {
	double x, y, l, b, r, t;
	double c, d, e; // Temporary variables

	x = y = 0.0;

	type_ = tMacro;
	render_commands_ = macro->Render(modifiers, modifier_count);

	for (auto render : render_commands_) {
		switch (render->command_) {
		case RenderCommand::gcRectangle:
			l = render->X;
			b = render->Y;
			r = render->X + render->W;
			t = render->Y + render->H;
			x = l;
			y = b;
			break;

		case RenderCommand::gcCircle:
			l = render->X - render->W / 2.0;
			b = render->Y - render->W / 2.0;
			r = render->X + render->W / 2.0;
			t = render->Y + render->W / 2.0;
			x = r;
			y = 0.0;
			break;

		case RenderCommand::gcBeginLine:
			l = render->X;
			b = render->Y;
			r = render->X;
			t = render->Y;
			x = l;
			y = b;
			break;

		case RenderCommand::gcLine:
			l = render->X;
			b = render->Y;
			r = render->X;
			t = render->Y;
			x = l;
			y = b;
			break;

		case RenderCommand::gcArc:
			c = x - render->X;   // Start relative to center
			d = y - render->Y;   // End relative to center
			e = sqrt(c * c + d * d); // Radius
			c = atan2(d, c) + render->A * kPi / 180.0; // Angle of end point

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
			left_ = l;
			bottom_ = b;
			right_ = r;
			top_ = t;
		}
		else {
			if (left_ > l) left_ = l;
			if (bottom_ > b) bottom_ = b;
			if (right_ < r) right_ = r;
			if (top_ < t) top_ = t;
		}
	}
}

bool GerberAperture::SolidCircle() {
	return(type_ == tCircle && hole_x_ < 0.0 && hole_y_ < 0.0);
}

bool GerberAperture::SolidRectangle() {
	return(type_ == tRectangle && hole_x_ < 0.0 && hole_y_ < 0.0);
}

void GerberAperture::Add(std::shared_ptr<RenderCommand> render) {
	render_commands_.push_back(render);
}

void GerberAperture::RenderHole() {
	std::shared_ptr<RenderCommand> render;

	if (hole_x_ > 0.0) {
		if (hole_y_ < 0.0) {
			render = std::make_shared<RenderCommand>(RenderCommand::gcBeginLine);
			render->X = hole_x_ / 2.0;
			render->Y = 0.0;
			Add(render);

			render = std::make_shared<RenderCommand>(RenderCommand::gcArc);
			render->X = 0.0;
			render->Y = 0.0;
			render->A = -360.0;
			Add(render);

		}
		else {
			render = std::make_shared<RenderCommand>(RenderCommand::gcBeginLine);
			render->X = hole_x_ / 2.0;
			render->Y = -hole_y_ / 2.0;
			Add(render);

			render = std::make_shared<RenderCommand>(RenderCommand::gcLine);
			render->X = -hole_x_ / 2.0;
			render->Y = -hole_y_ / 2.0;
			Add(render);

			render = std::make_shared<RenderCommand>(RenderCommand::gcLine);
			render->X = -hole_x_ / 2.0;
			render->Y = hole_y_ / 2.0;
			Add(render);

			render = std::make_shared<RenderCommand>(RenderCommand::gcLine);
			render->X = hole_x_ / 2.0;
			render->Y = hole_y_ / 2.0;
			Add(render);

			render = std::make_shared<RenderCommand>(RenderCommand::gcClose);
			Add(render);
		}
	}
}

void GerberAperture::RenderObround() {
	std::shared_ptr<RenderCommand> render;

	double r, t;

	if (dimension_x_ > dimension_y_) {
		r = dimension_y_ / 2.0;
		t = dimension_x_ / 2.0 - r;

		render = std::make_shared<RenderCommand>(RenderCommand::gcBeginLine);
		render->X = t;
		render->Y = -r;
		Add(render);

		render = std::make_shared<RenderCommand>(RenderCommand::gcArc);
		render->X = t;
		render->Y = 0.0;
		render->A = 180.0;
		Add(render);

		render = std::make_shared<RenderCommand>(RenderCommand::gcLine);
		render->X = -t;
		render->Y = r;
		Add(render);

		render = std::make_shared<RenderCommand>(RenderCommand::gcArc);
		render->X = -t;
		render->Y = 0.0;
		render->A = 180.0;
		Add(render);

	}
	else {
		r = dimension_x_ / 2.0;
		t = dimension_y_ / 2.0 - r;

		render = std::make_shared<RenderCommand>(RenderCommand::gcBeginLine);
		render->X = r;
		render->Y = t;
		Add(render);

		render = std::make_shared<RenderCommand>(RenderCommand::gcArc);
		render->X = 0.0;
		render->Y = t;
		render->A = 180.0;
		Add(render);

		render = std::make_shared<RenderCommand>(RenderCommand::gcLine);
		render->X = -r;
		render->Y = -t;
		Add(render);

		render = std::make_shared<RenderCommand>(RenderCommand::gcArc);
		render->X = 0.0;
		render->Y = -t;
		render->A = 180.0;
		Add(render);
	}

	render = std::make_shared<RenderCommand>(RenderCommand::gcClose);
	Add(render);

	RenderHole();

	render = std::make_shared<RenderCommand>(RenderCommand::gcFill);
	Add(render);
}

void GerberAperture::RenderCircle() {
	std::shared_ptr<RenderCommand> render;

	render = std::make_shared<RenderCommand>(RenderCommand::gcCircle);
	render->X = 0.0;
	render->Y = 0.0;
	render->W = dimension_x_;
	Add(render);

	RenderHole();

	render = std::make_shared<RenderCommand>(RenderCommand::gcFill);
	Add(render);
}

void GerberAperture::RenderPolygon() {
	double        r, a, da, rot, lim;
	std::shared_ptr<RenderCommand> render;

	// Get Rotation in range [0; 360)
	while (rotation_ < 0.0) rotation_ += 360;
	while (rotation_ >= 360.0) rotation_ -= 360;

	r = dimension_x_ / 2.0;
	da = 2.0 * kPi / side_count_;
	rot = rotation_ * kPi / 180.0;
	lim = 2.0 * kPi - da / 2.0;

	render = std::make_shared<RenderCommand>(RenderCommand::gcBeginLine);
	render->X = r * cos(rot);
	render->Y = r * sin(rot);
	Add(render);

	for (a = da; a < lim; a += da) {
		render = std::make_shared<RenderCommand>(RenderCommand::gcLine);
		render->X = r * cos(a + rot);
		render->Y = r * sin(a + rot);
		Add(render);
	}

	render = std::make_shared<RenderCommand>(RenderCommand::gcClose);
	Add(render);

	RenderHole();

	render = std::make_shared<RenderCommand>(RenderCommand::gcFill);
	Add(render);
}

void GerberAperture::RenderRectangle() {
	std::shared_ptr<RenderCommand> render;

	render = std::make_shared<RenderCommand>(RenderCommand::gcRectangle);
	render->X = -dimension_x_ / 2.0;
	render->Y = -dimension_y_ / 2.0;
	render->W = dimension_x_;
	render->H = dimension_y_;
	Add(render);

	RenderHole();

	render = std::make_shared<RenderCommand>(RenderCommand::gcFill);
	Add(render);
}

void GerberAperture::RenderAperture() {
	switch (type_) {
	case tCircle:
		RenderCircle();
		break;

	case tRectangle:
		RenderRectangle();
		break;

	case tObround:
		RenderObround();
		break;

	case tPolygon:
		RenderPolygon();
		break;

	case tMacro:
	default:
		break;
	}
}

std::vector<std::shared_ptr<RenderCommand>> GerberAperture::Render() {
	if (render_commands_.empty())
		RenderAperture();

	return render_commands_;
}
