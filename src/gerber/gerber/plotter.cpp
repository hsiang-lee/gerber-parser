#include "plotter.h"
#include "gerber/gerber_level.h"
#include "gerber/aperture/aperture.h"
#include <glog/logging.h>

#include <array>


constexpr double kPi = 3.141592653589793238463;

Plotter::Plotter(GerberLevel& level, std::shared_ptr<Plotter> old_plotter) :level_(level) {
	x_ = y_ = i_ = j_ = 0.0;
	multi_quadrant_ = false;

	exposure_ = geOff;
	interpolation_ = giLinear;

	if (old_plotter) {
		exposure_ = old_plotter->exposure_;
		multi_quadrant_ = old_plotter->multi_quadrant_;
		interpolation_ = old_plotter->interpolation_;
		current_aperture_ = old_plotter->current_aperture_;

		pre_x_ = old_plotter->pre_x_;
		pre_y_ = old_plotter->pre_y_;
		x_ = old_plotter->x_;
		y_ = old_plotter->y_;

		drawing_line_ = old_plotter->drawing_line_;
	}
}

void Plotter::OutlineBegin(unsigned line_number) {
	exposure_ = geOff;
	Move(line_number);
	level_.Add(std::make_shared<BeginOutlineCommand>());
	drawing_outline_ = true;
}

void Plotter::OutlineEnd(unsigned line_number) {
	exposure_ = geOff;
	Move(line_number);
	level_.Add(std::make_shared<EndOutlineCommand>());
	drawing_outline_ = false;
}

void Plotter::Do(unsigned line_number) {
	switch (exposure_) {
	case geOn:
		Line();
		break;

	case geOff:
		Move(line_number);
		break;

	case geFlash:
		Move(line_number);
		Flash();
		exposure_ = geOff;
		break;

	default:
		break;
	}

	i_ = j_ = 0.0;
}

void Plotter::ApertureSelect(const std::shared_ptr<Aperture>& aperture, unsigned line_number) {
	exposure_ = geOff;
	Move(line_number);

	level_.Add(std::make_shared<ApertureSelectCommand>(aperture));
	current_aperture_ = aperture;
}

void Plotter::Move(unsigned line_number) {
	if (drawing_line_) {
		if (drawing_outline_) {
			if (first_x_ != pre_x_ || first_y_ != pre_y_) {
				LOG(WARNING) << "Line " << line_number << " - Warning: Deprecated feature: Open contours";
				level_.Add(std::make_shared<CloseCommand>());
			}
			auto tmp = std::make_shared<FillCommand>();
			level_.Add(tmp);
			tmp->end_.first = pre_x_;
			tmp->end_.second = pre_y_;

		}
		else {
			auto tmp = std::make_shared<StrokeCommand>();
			level_.Add(tmp);
			tmp->end_.first = pre_x_;
			tmp->end_.second = pre_y_;
		}
	}

	drawing_line_ = false;

	first_x_ = UnitType::Get_mm(x_, level_.unit_);
	first_y_ = UnitType::Get_mm(y_, level_.unit_);
	pre_x_ = first_x_;
	pre_y_ = first_y_;
}

void Plotter::Line() {
	if (!drawing_line_) {
		if (current_aperture_ && !drawing_outline_) {
			level_.bound_box_.UpdateBox(
				pre_x_ + current_aperture_->bound_box_.Left(),
				pre_x_ + current_aperture_->bound_box_.Right(),
				pre_y_ + current_aperture_->bound_box_.Top(),
				pre_y_ + current_aperture_->bound_box_.Bottom());
		}
		else {
			level_.bound_box_.UpdateBox(pre_x_, pre_x_, pre_y_, pre_y_);
		}

		auto tmp = std::make_shared<BeginLineCommand>(pre_x_, pre_y_);
		level_.Add(tmp);
		tmp->end_.first = tmp->X = pre_x_;
		tmp->end_.second = tmp->Y = pre_y_;
	}
	else {
		if (pre_x_ == UnitType::Get_mm(x_, level_.unit_) && pre_y_ == UnitType::Get_mm(y_, level_.unit_) && i_ == 0.0 && j_ == 0.0) {
			return;
		}
	}
	drawing_line_ = true;

	switch (interpolation_) {
	case giLinear: {
		auto tmp = std::make_shared<LineCommand>(UnitType::Get_mm(x_, level_.unit_), UnitType::Get_mm(y_, level_.unit_));
		level_.Add(tmp);
		tmp->end_.first = tmp->X = UnitType::Get_mm(x_, level_.unit_);
		tmp->end_.second = tmp->Y = UnitType::Get_mm(y_, level_.unit_);
		break;
	}
	case giLinear10X: {
		auto tmp = std::make_shared<LineCommand>(UnitType::Get_mm(x_, level_.unit_) * 10.0, UnitType::Get_mm(y_, level_.unit_) * 10.0);
		level_.Add(tmp);
		tmp->end_.first = tmp->X = UnitType::Get_mm(x_, level_.unit_) * 10.0;// NOLINT
		tmp->end_.second = tmp->Y = UnitType::Get_mm(y_, level_.unit_) * 10.0;// NOLINT
		break;
	}
	case giLinear0_1X: {
		auto tmp = std::make_shared<LineCommand>(UnitType::Get_mm(x_, level_.unit_) * 0.1, UnitType::Get_mm(y_, level_.unit_) * 0.1);
		level_.Add(tmp);
		tmp->end_.first = tmp->X = UnitType::Get_mm(x_, level_.unit_) * 0.1;// NOLINT
		tmp->end_.second = tmp->Y = UnitType::Get_mm(y_, level_.unit_) * 0.1;// NOLINT
		break;
	}
	case giLinear0_01X: {
		auto tmp = std::make_shared<LineCommand>(UnitType::Get_mm(x_, level_.unit_) * 0.01, UnitType::Get_mm(y_, level_.unit_) * 0.01);
		level_.Add(tmp);
		tmp->end_.first = tmp->X = UnitType::Get_mm(x_, level_.unit_) * 0.01;// NOLINT
		tmp->end_.second = tmp->Y = UnitType::Get_mm(y_, level_.unit_) * 0.01;// NOLINT
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

	pre_x_ = UnitType::Get_mm(x_, level_.unit_);
	pre_y_ = UnitType::Get_mm(y_, level_.unit_);

	if (current_aperture_ && !drawing_outline_) {
		level_.bound_box_.UpdateBox(
			pre_x_ + current_aperture_->bound_box_.Left(),
			pre_x_ + current_aperture_->bound_box_.Right(),
			pre_y_ + current_aperture_->bound_box_.Top(),
			pre_y_ + current_aperture_->bound_box_.Bottom());
	}
	else {
		level_.bound_box_.UpdateBox(pre_x_, pre_x_, pre_y_, pre_y_);
	}
}


void Plotter::Arc() {
	double x1 = 0.0;
	double y1 = 0.0; // Start, relative to center
	double x2 = 0.0;
	double y2 = 0.0; // End, relative to center
	double x3 = 0.0;
	double y3 = 0.0; // Center

	if (multi_quadrant_) {
		x1 = -UnitType::Get_mm(i_, level_.unit_);
		y1 = -UnitType::Get_mm(j_, level_.unit_);

		x2 = UnitType::Get_mm(x_, level_.unit_) - pre_x_ - UnitType::Get_mm(i_, level_.unit_);
		y2 = UnitType::Get_mm(y_, level_.unit_) - pre_y_ - UnitType::Get_mm(j_, level_.unit_);

		x3 = pre_x_ + UnitType::Get_mm(i_, level_.unit_);
		y3 = pre_y_ + UnitType::Get_mm(j_, level_.unit_);
	}
	else {
		std::array<double, 4> i{};
		std::array<double, 4> j{};

		i[0] = UnitType::Get_mm(i_, level_.unit_);
		j[0] = UnitType::Get_mm(j_, level_.unit_);
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
			auto a = GetAngle(-i[t], -j[t], UnitType::Get_mm(x_, level_.unit_) - pre_x_ - i[t], UnitType::Get_mm(y_, level_.unit_) - pre_y_ - j[t]);
			if (interpolation_ == giClockwiseCircular) {
				if (a > 0.0) {
					continue;
				}
			}
			else { // CCW
				if (a < 0.0) {
					continue;
				}
			}

			x2 = UnitType::Get_mm(x_, level_.unit_) - pre_x_ - i[t];
			y2 = UnitType::Get_mm(y_, level_.unit_) - pre_y_ - j[t];
			const auto error = fabs((i[t] * i[t] + j[t] * j[t]) - (x2 * x2 + y2 * y2));
			if (error < Error) {
				T = t;
				Error = error;
			}
		}

		x3 = pre_x_ + i[T];
		y3 = pre_y_ + j[T];

		x1 = -i[T];
		y1 = -j[T];
		x2 = UnitType::Get_mm(x_, level_.unit_) - x3;
		y2 = UnitType::Get_mm(y_, level_.unit_) - y3;
	}

	const auto angle = GetAngle(x1, y1, x2, y2);

	auto tmp = std::make_shared<ArcCommand>(x3, y3, angle);
	level_.Add(tmp);
	tmp->end_.first = pre_x_ = UnitType::Get_mm(x_, level_.unit_);
	tmp->end_.second = pre_y_ = UnitType::Get_mm(y_, level_.unit_);

	auto right = x3 + x1;
	auto top = y3 + y1;
	auto left = right;
	auto bottom = top;

	auto d = atan2(y1, x1);
	const auto rad = sqrt(x1 * x1 + y1 * y1); // Radius
	const auto dd = angle * kPi / 180e3;
	for (int j = 0; j < 1000; ++j) {// NOLINT
		const auto x = x3 + rad * cos(d);
		const auto y = y3 + rad * sin(d);
		if (left > x)
		{
			left = x;
		}

		if (bottom > y) {
			bottom = y;
		}

		if (right < x) {
			right = x;
		}

		if (top < y) {
			top = y;
		}

		d += dd;
	}

	if (current_aperture_ && !drawing_outline_) {
		left += current_aperture_->bound_box_.Left();
		bottom += current_aperture_->bound_box_.Bottom();
		right += current_aperture_->bound_box_.Right();
		top += current_aperture_->bound_box_.Top();
	}

	level_.bound_box_.UpdateBox(left, right, top, bottom);
}

void Plotter::Flash() {
	level_.Add(std::make_shared<FlashCommand>(pre_x_, pre_y_));

	if (current_aperture_) {
		level_.bound_box_.UpdateBox(
			pre_x_ + current_aperture_->bound_box_.Left(),
			pre_x_ + current_aperture_->bound_box_.Right(),
			pre_y_ + current_aperture_->bound_box_.Top(),
			pre_y_ + current_aperture_->bound_box_.Bottom()
		);
	}
}

double Plotter::GetAngle(
	double x1, double y1, // Start, relative to center
	double x2, double y2  // End, relative to center
) {
	double a = (atan2(y2, x2) - atan2(y1, x1)) * 180.0 / kPi; // [-360; 360]

	if (interpolation_ == giClockwiseCircular) { // CW
		while (a >= 0.0) {
			a -= 360.0; // NOLINT
		}

		if (!multi_quadrant_ && a < -90.001) {// NOLINT
			a += 360; // NOLINT
		}
	}
	else { // CCW
		while (a <= 0.0) {
			a += 360.0; // NOLINT
		}

		if (!multi_quadrant_ && a > 90.001) {// NOLINT
			a -= 360; // NOLINT
		}
	}

	if (multi_quadrant_) {
		if (fabs(a) < 0.001) {// NOLINT
			if (a >= 0.0) {// NOLINT
				a += 360;// NOLINT
			}
			else {
				a -= 360;// NOLINT
			}
		}
	}

	return a;
}
