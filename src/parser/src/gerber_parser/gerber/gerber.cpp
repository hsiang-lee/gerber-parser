#include "gerber.h"
#include "gerber/gerber_layer.h"
#include "aperture/aperture.h"
#include <algorithm>

constexpr double kVeryLarge = 1e5;
constexpr double kVeryLittle = -1e5;

Gerber::~Gerber()
{
    for (auto each : layers_)
    {
        delete each;
    }

    for (auto each : apertures_)
    {
        delete each;
    }

    layers_.clear();
    apertures_.clear();
}

double UnitType::Get_mm(double number, Type unit)
{
    if (unit == guInches)
    {
        number *= 25.4; // NOLINT
    }

    return number;
}

bool Gerber::IsNegative() const
{
    return negative_;
}

BoundBox Gerber::GetBBox() const
{
    if (layers_.empty())
    {
        return {0.0, 0.0, 0.0, 0.0};
    }

    BoundBox bound_box(kVeryLarge, kVeryLittle, kVeryLittle, kVeryLarge);
    std::for_each(layers_.begin(), layers_.end(), [&bound_box](GerberLayer *layer)
                  { bound_box.Update(layer->GetBBox().Left(), layer->GetRight(), layer->GetTop(), layer->GetBBox().Bottom()); });

    return bound_box;
}

UnitType::Type Gerber::Unit() const
{
    return unit_;
}

std::string Gerber::Name() const
{
    return name_;
}

Aperture *Gerber::GetAperture(int code) const
{
    return apertures_[code];
}

void Gerber::SetAperture(int code, Aperture *aperture)
{
    apertures_[code] = aperture;
}

const std::vector<GerberLayer *> &Gerber::GetLayers() const
{
    return layers_;
}

void Gerber::AddLayer(GerberLayer *layer)
{
    layers_.emplace_back(layer);
}

void Gerber::SetName(const std::string &name)
{
    name_ = name;
}

const std::vector<Aperture*>& Gerber::GetApertureList() const {
    return apertures_;
}
