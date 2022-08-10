#pragma once
#include <memory>
#include "gerber/gerber_enums.h"

class GerberLevel;
class Aperture;

class Plotter {
public:
	Plotter(GerberLevel& level, std::shared_ptr<Plotter> old_plotter);
	~Plotter() = default;

	void OutlineBegin(unsigned line_number);
	void OutlineEnd(unsigned line_number);
	void Do(unsigned line_number);
	void ApertureSelect(const std::shared_ptr<Aperture>& aperture, unsigned line_number);

	void SetExposure(GERBER_EXPOSURE exposure) {
		exposure_ = exposure;
	}

	void SetInterpolation(GERBER_INTERPOLATION interpolation) {
		interpolation_ = interpolation;
	}

	double x_, y_, i_, j_;
	bool multi_quadrant_;

private:
	void Move(unsigned line_number);
	void Line();
	void Arc();
	void Flash();

	double GetAngle(
		double x1, double y1, // Start, relative to center
		double x2, double y2  // End, relative to center
	);

	GerberLevel& level_;
	GERBER_EXPOSURE exposure_{ geOff };
	GERBER_INTERPOLATION interpolation_{ giLinear };

	bool drawing_line_{ false };
	bool drawing_outline_{ false };
	double pre_x_{ 0.0 }, pre_y_{ 0.0 };
	double first_x_{ 0.0 }, first_y_{ 0.0 };

	std::shared_ptr<Aperture> current_aperture_;
};
