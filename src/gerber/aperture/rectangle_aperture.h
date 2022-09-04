#pragma once
#include "aperture.h"

class RectangleAperture : public Aperture {
public:
	RectangleAperture(int code, double w, double h, double x, double y);

	void RenderAperture() override;
	bool SolidCircle() override;
	bool SolidRectangle() override;
};
