#pragma once

#include <string>
#include <vector>
#include <memory>

class Primitive;
class PrimitiveItem;
class GerberMacroParser;

class GerberMacro
{
private:
    std::vector<std::shared_ptr<PrimitiveItem>> primitive_items_;
    void Add(const std::shared_ptr<PrimitiveItem> &item);

    std::vector<Primitive *> primitives_;
    void Add(Primitive *render);

    std::string name_;
    std::vector<double> modifiers_;
    bool exposure_;

    double Get_mm(double number) const;

    bool inches_;

    friend class PrimitiveItem;
    friend class GerberMacroParser;
    friend class CommentPrimitiveItem;
    friend class CirclePrimitiveItem;
    friend class LineVectorPrimitiveItem;
    friend class LineVector2PrimitiveItem;
    friend class LineCenterPrimitiveItem;
    friend class LineLowerLeftPrimitiveItem;
    friend class EndOfFilePrimitiveItem;
    friend class OutlinePrimitiveItem;
    friend class PolygonPrimitiveItem;
    friend class MoirePrimitiveItem;
    friend class ThermalPrimitiveItem;
    friend class AssignmentPrimitiveItem;

public:
    GerberMacro(const std::string &name, bool inches);
    ~GerberMacro();

    std::string Name() const;
    std::vector<Primitive *> Render(const std::vector<double> &modifiers);
};
