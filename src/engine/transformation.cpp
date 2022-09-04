#include "transformation.h"



Transformation::Transformation(const BoundBox& box, const BoundBox& offset) :
	bound_box_(box),
	offset_(offset)
{
}

void Transformation::Move(int delta_x, int delta_y) {
	move_.first += delta_x;
	move_.second += delta_y;
}

bool Transformation::Scale(double delta, double center_x, double center_y) {
	if (scaled_ + delta >= 0.1 && scaled_ + delta <= 20) {// NOLINT
		left_top_.first = GetLeft(center_x, delta);
		left_top_.second = GetTop(center_y, delta);

		scaled_ += delta;
		with_scale_ = true;

		return true;
	}

	return false;
}

void Transformation::SetPhysicalSize(int width, int height) {
	physical_.first = width;
	physical_.second = height;

	if (!with_scale_) {
		left_top_.first = LogicLeft();
		left_top_.second = LogicTop();
	}
}

double Transformation::TranslatePenWidth(double width) const {
	return width / GetScaleRatio();
}

double Transformation::TranslateLogicCoord(double coord) const {
	return coord * GetScaleRatio();
}

double Transformation::ScaleY() const {
	return physical_.second * 0.95 / bound_box_.Height() * scaled_;
}

double Transformation::LogicLeft() const {
	const auto scale_x = ScaleX();
	const auto scale_y = ScaleY();
	if (scale_x < scale_y) {
		return bound_box_.Left();
	}

	return bound_box_.Left() - (bound_box_.Width() * (scale_x / scale_y - 1) / 2);
}

double Transformation::LogicTop() const {
	const auto scale_x = ScaleX();
	const auto scale_y = ScaleY();
	if (scale_x < scale_y) {
		return bound_box_.Top() + bound_box_.Height() * (scale_y / scale_x - 1) / 2;
	}

	return bound_box_.Top();
}

double Transformation::LogicWidth() const {
	return bound_box_.Width() * WidthRatio();
}

double Transformation::WidthRatio() const {
	if (!WScaleLargerThanHeightScale()) {
		return 1.0;
	}

	return physical_.first / physical_.second * bound_box_.Height() / bound_box_.Width();
}

double Transformation::HeightRatio() const {
	if (WScaleLargerThanHeightScale()) {
		return 1.0;
	}

	return physical_.second / physical_.first * bound_box_.Width() / bound_box_.Height();
}

bool Transformation::WScaleLargerThanHeightScale() const {
	return ScaleX() >= ScaleY();
}

double Transformation::GetLeft(double center_x, double scale_delta) const {
	if (WScaleLargerThanHeightScale()) {
		const auto old_width = LogicWidth() / scaled_;
		const auto width = LogicWidth() / (scaled_ + scale_delta);
		return left_top_.first + (old_width - width) * center_x;
	}

	return left_top_.first + bound_box_.Width() * (1.0 / scaled_ - 1.0 / (scaled_ + scale_delta)) * center_x;
}

double Transformation::GetTop(double center_y, double scale_delta) const {
	if (WScaleLargerThanHeightScale()) {
		return left_top_.second - bound_box_.Height() * (1.0 / scaled_ - 1.0 / (scaled_ + scale_delta)) * center_y;
	}

	const auto old_height = LogicHeight() / scaled_;
	const auto height = LogicHeight() / (scaled_ + scale_delta);
	return left_top_.second - (old_height - height) * center_y;
}

double Transformation::LogicHeight() const {
	return bound_box_.Height() * HeightRatio();
}

double Transformation::GetScaleRatio() const {
	return std::min(ScaleX(), ScaleY());
}

double Transformation::ScaleX() const {
	return physical_.first * 0.95 / bound_box_.Width() * scaled_;
}

QRect Transformation::GetPainterWindow() {
	return QRect(left_top_.first, left_top_.second, LogicWidth() / scaled_, -LogicHeight() / scaled_);
}

QRect Transformation::GetPainterViewport() const {
	return QRect(
		physical_.first * offset_.Left() + move_.first,
		physical_.second * offset_.Top() + move_.second,
		physical_.first * (1 - offset_.Left() - offset_.Right()),
		physical_.second * (1 - offset_.Top() - offset_.Bottom())
	);
}


