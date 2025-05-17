#pragma once
#include <memory>
#include "aperture.h"

class GerberMacro;

class MacroAperture : public Aperture
{
public:
    MacroAperture(int code, const std::shared_ptr<GerberMacro> &macro, const std::vector<double> &modifier);

    BoundBox BBox() const override;
    ApertureType Type() const override;

private:
    void RenderAperture() override;
    bool SolidCircle() override;
    bool SolidRectangle() override;
};
