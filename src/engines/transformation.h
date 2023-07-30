#pragma once
#include <QRect>
#include "gerber_parser/bound_box.h"

class Transformation
{
public:
    Transformation(const BoundBox &box, double offset);

    QRect GetPainterWindow();
    QRect GetPainterViewport() const;

    void SetPhysicalSize(int width, int height);

private:
    double LogicLeft() const;
    double LogicTop() const;
    double LogicWidth() const;
    double LogicHeight() const;

    double WidthRatio() const;
    double HeightRatio() const;

    std::pair<double, double> physical_{0.0, 0.0};

    BoundBox bound_box_{0.0, 0.0, 0.0, 0.0};
    double offset_;
};
