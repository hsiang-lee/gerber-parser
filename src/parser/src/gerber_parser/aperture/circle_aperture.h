#pragma once
#include "aperture.h"

class CircleAperture : public Aperture
{
public:
    CircleAperture(int code, double d, double x, double y);

    BoundBox BBox() const override;
    ApertureType Type() const override;

private:
    void RenderAperture() override;
    bool SolidCircle() override;
    bool SolidRectangle() override;
};
