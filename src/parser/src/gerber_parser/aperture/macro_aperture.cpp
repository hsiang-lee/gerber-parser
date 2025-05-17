#include "macro_aperture.h"
#include "gerber/gerber_primitive.h"
#include "aperture/gerber_macro.h"

constexpr double kPi = 3.141592653589793238463;
constexpr double kTwo = 2.0;

MacroAperture::MacroAperture(
    int code,
    const std::shared_ptr<GerberMacro> &macro,
    const std::vector<double> &modifier) : Aperture(code)
{
    primitives_ = macro->Render(modifier);
}

BoundBox MacroAperture::BBox() const
{
    BoundBox bound_box;
    for (auto &render : primitives_)
    {
        bound_box.Update(render->BBox());
    }

    return bound_box;
}

Aperture::ApertureType MacroAperture::Type() const
{
    return ApertureType::tMacro;
}

void MacroAperture::RenderAperture()
{
}

bool MacroAperture::SolidCircle()
{
    return false;
}

bool MacroAperture::SolidRectangle()
{
    return false;
}
