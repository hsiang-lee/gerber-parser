#include "transformation.h"


void Transformation::Move(int delta_x, int delta_y)
{
	move_x_ += delta_x;
	move_y_ += delta_y;
}

bool Transformation::Scale(double delta, double center_x, double center_y)
{
	old_scaled_ = scaled_;

	if (scaled_ + delta >= 0.1 && scaled_ + delta <= 20) {
		scaled_ += delta;
		center_x_ = center_x;
		center_y_ = center_y;

		return true;
	}

	return false;
}

void Transformation::SetPhysicalSize(int width, int height)
{
	physical_width_ = width;
	physical_height_ = height;

	if (!inited_) {
		old_left_ = LogicLeft();
		old_top_ = LogicTop();
		inited_ = true;
	}
}

double Transformation::TranslatePenWidth(double width) const
{
	return width / GetScaleRatio();
}

double Transformation::TranslateLogicCoord(double coord) const
{
	return coord * GetScaleRatio();
}

double Transformation::ScaleY() const
{
	return physical_height_ * 0.95 / bound_box_.Height() * scaled_;
}

double Transformation::LogicLeft() const
{
	const auto scale_x = ScaleX();
	const auto scale_y = ScaleY();
	if (scale_x < scale_y) {
		return bound_box_.Left();
	}

	return bound_box_.Left() - (bound_box_.Width() * (scale_x / scale_y - 1) / 2);
}

double Transformation::LogicTop() const
{
	const auto scale_x = ScaleX();
	const auto scale_y = ScaleY();
	if (scale_x < scale_y) {
		return bound_box_.Top() + bound_box_.Height() * (scale_y / scale_x - 1) / 2;
	}

	return bound_box_.Top();
}

double Transformation::LogicWidth() const
{
	return bound_box_.Width() * WidthRatio();
}

double Transformation::WidthRatio() const
{
	if (!WScaleLargerThanHeightScale()) {
		return 1.0;
	}

	return physical_width_ / physical_height_ * bound_box_.Height() / bound_box_.Width();
}

double Transformation::HeightRatio() const
{
	if (WScaleLargerThanHeightScale()) {
		return 1.0;
	}

	return physical_height_ / physical_width_ * bound_box_.Width() / bound_box_.Height();
}

bool Transformation::WScaleLargerThanHeightScale() const
{
	return ScaleX() >= ScaleY();
}

double Transformation::GetLeft() const
{
	if (WScaleLargerThanHeightScale()) {
		const auto old_width = LogicWidth() / old_scaled_;
		const auto width = LogicWidth() / scaled_;
		return old_left_ + (old_width - width) * center_x_;
	}

	return old_left_ + bound_box_.Width() * (1.0 / old_scaled_ - 1.0 / scaled_) * center_x_;
}

double Transformation::GetTop() const
{
	if (WScaleLargerThanHeightScale()) {
		return old_top_ - bound_box_.Height() * (1.0 / old_scaled_ - 1.0 / scaled_) * center_y_;
	}

	const auto old_height = LogicHeight() / old_scaled_;
	const auto height = LogicHeight() / scaled_;
	return old_top_ - (old_height - height) * center_y_;
}

double Transformation::LogicHeight() const
{
	return bound_box_.Height() * HeightRatio();
}

double Transformation::GetScaleRatio() const
{
	return std::min(ScaleX(), ScaleY());
}

double Transformation::ScaleX() const
{
	return physical_width_ * 0.95 / bound_box_.Width() * scaled_;
}

QRect Transformation::GetPainterWindow()
{
	old_left_ = GetLeft();
	old_top_ = GetTop();
	return QRect(old_left_, old_top_, LogicWidth() / scaled_, -LogicHeight() / scaled_);
}

QRect Transformation::GetPainterViewport() const
{
	return QRect(
		physical_width_ * 0.025 + move_x_,
		physical_height_ * 0.025 + move_y_,
		physical_width_ * 0.95,
		physical_height_ * 0.95
	);
}


