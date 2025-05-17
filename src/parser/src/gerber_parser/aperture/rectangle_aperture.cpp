#include "rectangle_aperture.h"
#include "gerber/gerber_primitive.h"

constexpr double kTwo = 2.0;

void RectangleAperture::RenderAperture()
{
    auto render = new Rectangle(dimension_x_, dimension_y_);
    render->start_ = std::make_pair(-dimension_x_ / kTwo, -dimension_y_ / kTwo);
    primitives_.emplace_back(render);

    RenderHole();
}

RectangleAperture::RectangleAperture(int code, double w, double h, double x, double y) : Aperture(code)
{
    dimension_x_ = w;
    dimension_y_ = h;

    if (x > 0.0)
    {
        if (y > 0.0)
        {
            HoleRectangle(x, y);
        }
        else
        {
            HoleCircle(x);
        }
    }
}

BoundBox RectangleAperture::BBox() const
{
    return BoundBox(-dimension_x_ / kTwo, dimension_x_ / kTwo, dimension_y_ / kTwo, -dimension_y_ / kTwo);
}

Aperture::ApertureType RectangleAperture::Type() const
{
    return ApertureType::tRectangle;
}

bool RectangleAperture::SolidCircle()
{
    return false;
}

bool RectangleAperture::SolidRectangle()
{
    return hole_x_ < 0.0 && hole_y_ < 0.0;
}
