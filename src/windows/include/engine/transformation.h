#pragma once
#include <QRect>
#include "gerber_parser/bound_box.h"


class Transformation {
public:
	Transformation(const BoundBox& box, const BoundBox& offset);

	QRect GetPainterWindow();
	QRect GetPainterViewport() const;

	void Move(int delta_x, int delta_y);
	bool Scale(double delta, double center_x = 0.0, double center_y = 0.0);
	void SetPhysicalSize(int width, int height);

	double TranslatePenWidth(double width) const;
	double TranslateLogicCoord(double coord) const;

private:
	double ScaleX() const;
	double ScaleY() const;
	double GetScaleRatio() const;

	double LogicLeft() const;
	double LogicTop() const;
	double LogicWidth() const;
	double LogicHeight() const;

	double WidthRatio() const;
	double HeightRatio() const;

	inline bool WScaleLargerThanHeightScale() const;

	double GetLeft(double center_x, double scale_delta) const;
	double GetTop(double center_y, double scale_delta) const;

	double scaled_{ 1.0 };
	bool with_scale_{ false };

	std::pair<double, double> left_top_{ 0.0,0.0 };
	std::pair<double, double> move_{ 0.0,0.0 };
	std::pair<double, double> physical_{ 0.0,0.0 };

	BoundBox bound_box_{ 0.0, 0.0, 0.0, 0.0 };
	BoundBox offset_{ 0.0, 0.0, 0.0, 0.0 };
};

