#pragma once
#include "aperture.h"

class GerberMacro;

class MacroAperture : public Aperture {
public:
	MacroAperture(int code, const std::shared_ptr<GerberMacro>& macro, const std::vector<double>& modifier);

protected:
	void RenderAperture() override;
	bool SolidCircle() override;
	bool SolidRectangle() override;
};
