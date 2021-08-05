#include "plotter.h"
#include "gerber/gerber_level.h"
#include "gerber/gerber_aperture.h"
#include <glog/logging.h>

constexpr double kPi = 3.141592653589793238463;

Plotter::Plotter(GerberLevel& level) :level_(level) {

}


void Plotter::OutlineBegin(unsigned line_number) {
	level_.exposure_ = geOff;
	Move(line_number);
	level_.AddNew(RenderCommand::gcBeginOutline);
	outline_fill_ = true;
}

void Plotter::OutlineEnd(unsigned line_number) {
	level_.exposure_ = geOff;
	Move(line_number);
	level_.AddNew(RenderCommand::gcEndOutline);
	outline_fill_ = false;
}

void Plotter::Do(unsigned line_number) {
	switch (level_.exposure_) {
	case geOn:
		Line();
		break;

	case geOff:
		Move(line_number);
		break;

	case geFlash:
		Move(line_number);
		Flash();
		level_.exposure_ = geOff;
		break;

	default:
		break;
	}

	level_.I = level_.J = 0.0;
}

void Plotter::ApertureSelect(std::shared_ptr<GerberAperture> aperture, unsigned line_number) {
	level_.exposure_ = geOff;
	Move(line_number);

	auto tmp = level_.AddNew(RenderCommand::gcApertureSelect);
	tmp->aperture_ = aperture;
	current_aperture = aperture;
}


void Plotter::Move(unsigned line_number) {
	if (in_path_) {
		if (outline_fill_) {
			if (firstX != preX || firstY != preY) {
				if (gerber_warnings) {
					LOG(WARNING) << "Line " << line_number << " - Warning: Deprecated feature: Open contours";
				}
				level_.AddNew(RenderCommand::gcClose);
			}
			auto tmp = level_.AddNew(RenderCommand::gcFill);
			tmp->End.X = preX;
			tmp->End.Y = preY;

		}
		else {
			auto tmp = level_.AddNew(RenderCommand::gcStroke);
			tmp->End.X = preX;
			tmp->End.Y = preY;
		}
	}

	in_path_ = false;

	firstX = Get_mm(level_.X);
	firstY = Get_mm(level_.Y);
	preX = firstX;
	preY = firstY;
}

void Plotter::Line() {
	if (!in_path_) {
		if (current_aperture && !outline_fill_) {
			level_.bound_box_.UpdateBox(
				preX + current_aperture->left_,
				preX + current_aperture->right_,
				preY + current_aperture->top_,
				preY + current_aperture->bottom_);
		}
		else {
			level_.bound_box_.UpdateBox(preX, preX, preY, preY);
		}

		auto tmp = level_.AddNew(RenderCommand::gcBeginLine);
		tmp->End.X = tmp->X = preX;
		tmp->End.Y = tmp->Y = preY;
	}
	else {
		if (
			preX == Get_mm(level_.X) && preY == Get_mm(level_.Y) &&
			level_.I == 0.0 && level_.J == 0.0
			) return;
	}
	in_path_ = true;

	switch (level_.interpolation_) {
	case giLinear: {
		auto tmp = level_.AddNew(RenderCommand::gcLine);
		tmp->End.X = tmp->X = Get_mm(level_.X);
		tmp->End.Y = tmp->Y = Get_mm(level_.Y);
		break;
	}
	case giLinear10X: {
		auto tmp = level_.AddNew(RenderCommand::gcLine);
		tmp->End.X = tmp->X = Get_mm(level_.X) * 10.0;
		tmp->End.Y = tmp->Y = Get_mm(level_.Y) * 10.0;
		break;
	}
	case giLinear0_1X: {
		auto tmp = level_.AddNew(RenderCommand::gcLine);
		tmp->End.X = tmp->X = Get_mm(level_.X) * 0.1;
		tmp->End.Y = tmp->Y = Get_mm(level_.Y) * 0.1;
		break;
	}
	case giLinear0_01X: {
		auto tmp = level_.AddNew(RenderCommand::gcLine);
		tmp->End.X = tmp->X = Get_mm(level_.X) * 0.01;
		tmp->End.Y = tmp->Y = Get_mm(level_.Y) * 0.01;
		break;
	}
	case giClockwiseCircular:
	case giCounterclockwiseCircular: {
		Arc();
		break;
	}
	default:
		break;
	}

	preX = Get_mm(level_.X);
	preY = Get_mm(level_.Y);

	if (current_aperture && !outline_fill_) {
		level_.bound_box_.UpdateBox(
			preX + current_aperture->left_,
			preX + current_aperture->right_,
			preY + current_aperture->top_,
			preY + current_aperture->bottom_);
	}
	else {
		level_.bound_box_.UpdateBox(preX, preX, preY, preY);
	}
}


void Plotter::Arc() {
	double x1, y1; // Start, relative to center
	double x2, y2; // End, relative to center
	double x3, y3; // Center

	if (level_.multi_quadrant_) {
		x1 = -Get_mm(level_.I);
		y1 = -Get_mm(level_.J);

		x2 = Get_mm(level_.X) - preX - Get_mm(level_.I);
		y2 = Get_mm(level_.Y) - preY - Get_mm(level_.J);

		x3 = preX + Get_mm(level_.I);
		y3 = preY + Get_mm(level_.J);
	}
	else {
		double i[4], j[4];

		i[0] = Get_mm(level_.I);
		j[0] = Get_mm(level_.J);
		i[1] = -i[0];
		j[1] = j[0];
		i[2] = -i[0];
		j[2] = -j[0];
		i[3] = i[0];
		j[3] = -j[0];

		// Find the best match for the centre coordinates
		// by minimising the error function.
		int T = 0;
		double Error = INFINITY;
		for (int t = 0; t < 4; ++t) {
			auto a = GetAngle(-i[t], -j[t], Get_mm(level_.X) - preX - i[t], Get_mm(level_.Y) - preY - j[t]);
			if (level_.interpolation_ == giClockwiseCircular) {
				if (a > 0.0) continue;
			}
			else { // CCW
				if (a < 0.0) continue;
			}

			x2 = Get_mm(level_.X) - preX - i[t];
			y2 = Get_mm(level_.Y) - preY - j[t];
			const auto error = fabs((i[t] * i[t] + j[t] * j[t]) - (x2 * x2 + y2 * y2));
			if (error < Error) {
				T = t;
				Error = error;
			}
		}

		x3 = preX + i[T];
		y3 = preY + j[T];

		x1 = -i[T];
		y1 = -j[T];
		x2 = Get_mm(level_.X) - x3;
		y2 = Get_mm(level_.Y) - y3;
	}

	const auto angle = GetAngle(x1, y1, x2, y2);

	auto tmp = level_.AddNew(RenderCommand::gcArc);
	tmp->X = x3;
	tmp->Y = y3;
	tmp->A = angle;
	tmp->End.X = preX = Get_mm(level_.X);
	tmp->End.Y = preY = Get_mm(level_.Y);

	auto r = x3 + x1;
	auto t = y3 + y1;
	auto l = r;
	auto b = t;

	auto d = atan2(y1, x1);
	const auto rad = sqrt(x1 * x1 + y1 * y1); // Radius
	const auto dd = angle * kPi / 180e3;
	for (int j = 0; j < 1000; ++j) {
		const auto x = x3 + rad * cos(d);
		const auto y = y3 + rad * sin(d);
		if (l > x)
		{
			l = x;
		}

		if (b > y) {
			b = y;
		}

		if (r < x) {
			r = x;
		}
		if (t < y) {
			t = y;
		}

		d += dd;
	}

	if (current_aperture && !outline_fill_) {
		l += current_aperture->left_;
		b += current_aperture->bottom_;
		r += current_aperture->right_;
		t += current_aperture->top_;
	}

	level_.bound_box_.UpdateBox(l, r, t, b);
}

void Plotter::Flash() {
	auto tmp = level_.AddNew(RenderCommand::gcFlash);
	tmp->X = preX;
	tmp->Y = preY;

	if (current_aperture) {
		level_.bound_box_.UpdateBox(
			preX + current_aperture->left_,
			preX + current_aperture->right_,
			preY + current_aperture->top_,
			preY + current_aperture->bottom_
		);
	}
}

double Plotter::Get_mm(double number) {
	if (level_.units_ == guInches) {
		number *= 25.4;
	}

	return number;
}

double Plotter::GetAngle(
	double x1, double y1, // Start, relative to center
	double x2, double y2  // End, relative to center
) {
	double a1 = atan2(y1, x1) * 180.0 / kPi;
	double a2 = atan2(y2, x2) * 180.0 / kPi;

	double a = a2 - a1; // [-360; 360]

	if (level_.interpolation_ == giClockwiseCircular) { // CW
		while (a >= 0.0) a -= 360.0; // [-360; 0)
		if (!level_.multi_quadrant_ && a < -90.001) a += 360; // [-90; 270)
	}
	else { // CCW
		while (a <= 0.0) a += 360.0; // (0; 360]
		if (!level_.multi_quadrant_ && a > 90.001) a -= 360; // (-270; 90]
	}

	if (level_.multi_quadrant_) {
		if (fabs(a) < 0.001) {
			if (a >= 0.0) a += 360;
			else         a -= 360;
		}
	}

	return a;
}
