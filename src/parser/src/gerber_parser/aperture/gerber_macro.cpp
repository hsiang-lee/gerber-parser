#include "gerber_macro.h"
#include "primitive_item.h"
#include "gerber/gerber_primitive.h"
#include "parser/macro_parser/gerber_macro_parser.h"

GerberMacro::GerberMacro(const std::string &name, bool inches) : name_(name), inches_(inches)
{
    exposure_ = true;
}

GerberMacro::~GerberMacro()
{
}

std::string GerberMacro::Name() const
{
    return name_;
}

void GerberMacro::Add(Primitive *render)
{
    primitives_.emplace_back(render);
}

double GerberMacro::Get_mm(double number) const
{
    if (inches_)
    {
        number *= 25.4; // NOLINT
    }

    return number;
}

std::vector<Primitive *> GerberMacro::Render(const std::vector<double> &modifiers)
{
    primitives_.clear();
    exposure_ = true;
    modifiers_ = modifiers;

    for (const auto &each : primitive_items_)
    {
        if (!each->Render(*this))
        {
            return {};
        }
    }

    return primitives_;
}

void GerberMacro::Add(const std::shared_ptr<PrimitiveItem> &item)
{
    if (!item)
    {
        return;
    }

    primitive_items_.emplace_back(item);
}
