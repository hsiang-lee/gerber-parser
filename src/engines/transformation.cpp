#include "transformation.h"

Transformation::Transformation(const BoundBox &box, double offset) : bound_box_(box),
                                                                    offset_(offset)
{
}

void Transformation::SetPhysicalSize(int width, int height)
{
    physical_.first = width;
    physical_.second = height;
}

double Transformation::LogicLeft() const
{
    const auto physical_w = physical_.first * (1 - 2 * offset_);
    const auto physical_h = physical_.second * (1 - 2 * offset_);
    const auto scale_x = physical_w / bound_box_.Width();
    const auto scale_y = physical_h / bound_box_.Height();
    if (scale_x < scale_y) {
        return bound_box_.Left();
    }

    double margin = (scale_x / scale_y * bound_box_.Width() - bound_box_.Width()) / 2;
    return bound_box_.Left() - margin;
}

double Transformation::LogicTop() const
{
    const auto physical_w = physical_.first * (1 - 2 * offset_);
    const auto physical_h = physical_.second * (1 - 2 * offset_);
    const auto scale_x = physical_w / bound_box_.Width();
    const auto scale_y = physical_h / bound_box_.Height();
    if (scale_x < scale_y) {
        double margin = bound_box_.Height() * (scale_y / scale_x - 1) / 2;
        return bound_box_.Top() + margin;
    }

    return bound_box_.Top();
}

double Transformation::LogicWidth() const
{
    return bound_box_.Width() * WidthRatio();
}

double Transformation::WidthRatio() const
{
    const auto physical_w = physical_.first * (1 - 2 * offset_);
    const auto physical_h = physical_.second * (1 - 2 * offset_);
    const auto scale_x = physical_w / bound_box_.Width();
    const auto scale_y = physical_h / bound_box_.Height();
    if (scale_x < scale_y) {
        return 1.0;
    }

    return scale_x / scale_y;
}

double Transformation::HeightRatio() const
{
    const auto physical_w = physical_.first * (1 - 2 * offset_);
    const auto physical_h = physical_.second * (1 - 2 * offset_);
    const auto scale_x = physical_w / bound_box_.Width();
    const auto scale_y = physical_h / bound_box_.Height();
    if (scale_x >= scale_y) {
        return 1.0;
    }

    return scale_y / scale_x;
}

double Transformation::LogicHeight() const
{
    return bound_box_.Height() * HeightRatio();
}

QRect Transformation::GetPainterWindow()
{
    auto debug = QRect(LogicLeft(), LogicTop(), LogicWidth(), -LogicHeight());
    return QRect(LogicLeft(), LogicTop(), LogicWidth(), -LogicHeight());
}

QRect Transformation::GetPainterViewport() const
{
    return QRect(
        physical_.first * offset_,
        physical_.second * offset_,
        physical_.first * (1 - 2 * offset_),
        physical_.second * (1 - 2 * offset_));
}
