#pragma once
#include "aperture.h"

class PolygonAperture : public Aperture
{
public:
    PolygonAperture(int code, double w, int n, double a, double x, double y); // Width; Side count; Rotation

    BoundBox BBox() const override;
    ApertureType Type() const override;

private:
    void RenderAperture() override;
    bool SolidCircle() override;
    bool SolidRectangle() override;

    int side_count_; // Number of sides
};
