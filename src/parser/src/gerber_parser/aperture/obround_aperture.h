#pragma once
#include "aperture.h"

class ObroundAperture : public Aperture
{
public:
    ObroundAperture(int code, double w, double h, double x, double y);

    BoundBox BBox() const override;
    ApertureType Type() const override;

private:
    void RenderAperture() override;
    bool SolidCircle() override;
    bool SolidRectangle() override;
};
