#pragma once
#include <QRect>
#include "bound_box.h"


class Transformation {
public:
	QRect GetPainterWindow();
	QRect GetPainterViewport() const;

	void Move(int delta_x, int delta_y);
	bool Scale(double delta, double center_x = 0.0, double center_y = 0.0);
	void SetPhysicalSize(int width, int height);

	double TranslatePenWidth(double width) const;
	double TranslateLogicCoord(double coord) const;

	BoundBox bound_box_{ 0.0, 0.0, 0.0, 0.0 };

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

	double GetLeft() const;
	double GetTop() const;

	double center_x_{ 0.0 };
	double center_y_{ 0.0 };
	double scaled_{ 1.0 };
	double old_scaled_{ 1.0 };
	double old_left_{ 0.0 };
	double old_top_{ 0.0 };

	bool inited_{ false };

	double move_x_{ 0 };
	double move_y_{ 0 };

	double physical_width_{ 0 };
	double physical_height_{ 0 };
};

