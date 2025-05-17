#include "obround_aperture.h"
#include "gerber/gerber_primitive.h"

constexpr double kTwo = 2.0;

ObroundAperture::ObroundAperture(int code, double w, double h, double x, double y) : Aperture(code)
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

BoundBox ObroundAperture::BBox() const
{
    return BoundBox(-dimension_x_ / kTwo, dimension_x_ / kTwo, dimension_y_ / kTwo, -dimension_y_ / kTwo);
}

Aperture::ApertureType ObroundAperture::Type() const
{
    return ApertureType::tObround;
}

void ObroundAperture::RenderAperture()
{
    outline_ = new Outline();

    if (dimension_x_ > dimension_y_)
    {
        auto r = dimension_y_ / kTwo;
        auto t = dimension_x_ / kTwo - r;

        outline_->items_.emplace_back(new Composite::BeginLineItem(t, -r));
        outline_->items_.emplace_back(new Composite::ArcItem(t, 0.0, 180.0));
        outline_->items_.emplace_back(new Composite::LineItem(-t, r));
        outline_->items_.emplace_back(new Composite::ArcItem(-t, 0.0, 180.0));
    }
    else
    {
        auto r = dimension_x_ / kTwo;
        auto t = dimension_y_ / kTwo - r;

        outline_->items_.emplace_back(new Composite::BeginLineItem(r, t));
        outline_->items_.emplace_back(new Composite::ArcItem(0.0, t, 180.0));
        outline_->items_.emplace_back(new Composite::LineItem(-r, -t));
        outline_->items_.emplace_back(new Composite::ArcItem(0.0, -t, 180.0));
    }

    primitives_.emplace_back(outline_);
    outline_ = nullptr;

    RenderHole();
}

bool ObroundAperture::SolidCircle()
{
    return false;
}

bool ObroundAperture::SolidRectangle()
{
    return false;
}
