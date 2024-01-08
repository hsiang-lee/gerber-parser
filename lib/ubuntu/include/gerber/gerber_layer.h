#pragma once

#include <string>
#include <vector>

#include "gerber/gerber.h"
#include "gerber_parser/bound_box.h"

class Plotter;
class Engine;
class Primitive;

class GerberApi GerberLayer
{
private:
    std::vector<Primitive *> primitives_{};
    void Add(Primitive *primitive);

    friend class Plotter;
    friend class StrokesToFillsConverter;

    std::string name_;
    bool negative_;
    BoundBox bound_box_{0.0, 0.0, 0.0, 0.0};
    UnitType::Type unit_{UnitType::guMillimeters};

    void ConvertStrokesToFills();

public:
    GerberLayer(GerberLayer *previous_layer,
                UnitType::Type units);
    ~GerberLayer();

    double GetRight() const;
    double GetTop() const;

    UnitType::Type GetUnit() const;
    void SetUnit(UnitType::Type type);

    // Step-and-Repeat
    int count_x_, count_y_;
    double step_x_, step_y_;
    bool IsCopyLayer() const;

    std::string GetName() const;
    void SetName(const std::string &name);

    bool IsNegative() const;
    void SetNegative(bool);

    BoundBox GetBBox() const;
    BoundBox FindPrimitive(const std::pair<double, double>& pt);
    std::vector<Primitive *> Primitives() const;

    int Draw(Engine *engine);
};
