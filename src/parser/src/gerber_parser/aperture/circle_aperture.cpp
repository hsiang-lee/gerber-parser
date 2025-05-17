#include "circle_aperture.h"
#include "gerber/gerber_primitive.h"

constexpr double kTwo = 2.0;

CircleAperture::CircleAperture(int code, double d, double x, double y) : Aperture(code)
{
    dimension_x_ = d;

    if (x <= 0.0)
    {
        return;
    }

    if (y > 0.0)
    {
        HoleRectangle(x, y);
    }
    else
    {
        HoleCircle(x);
    }
}

BoundBox CircleAperture::BBox() const
{
    return BoundBox(-dimension_x_ / kTwo, dimension_x_ / kTwo, dimension_x_ / kTwo, -dimension_x_ / kTwo);
}

Aperture::ApertureType CircleAperture::Type() const
{
    return ApertureType::tCircle;
}

void CircleAperture::RenderAperture()
{
    auto render = new Circle(0.0, 0.0, dimension_x_);
    primitives_.emplace_back(render);
    RenderHole();
}

bool CircleAperture::SolidCircle()
{
    return hole_x_ < 0.0 && hole_y_ < 0.0;
}

bool CircleAperture::SolidRectangle()
{
    return false;
}
