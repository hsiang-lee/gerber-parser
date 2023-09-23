#pragma once

#include <string>
#include <vector>

#include "gerber_parser/bound_box.h"

class Aperture;
class GerberLayer;

class UnitType
{
public:
    enum Type
    {
        guInches,
        guMillimeters
    };

    static double Get_mm(double number, Type unit);
};

class GerberApi Gerber
{
public:
    Gerber()
    {
        unit_ = UnitType::guInches;
        apertures_.resize(3000); // NOLINT
    }

    ~Gerber();

    bool negative_{false};
    bool IsNegative() const;

    UnitType::Type unit_{UnitType::guMillimeters};
    UnitType::Type Unit() const;

    BoundBox GetBBox() const;
    std::string Name() const;
    void SetName(const std::string &name);

    Aperture *GetAperture(int code) const;
    void SetAperture(int code, Aperture *aperture);

    const std::vector<GerberLayer *> &GetLayers() const;
    void AddLayer(GerberLayer *layer);

private:
    std::vector<GerberLayer *> layers_;
    std::vector<Aperture *> apertures_;
    std::string name_;
};
