#pragma once
#include "aperture.h"



class CircleAperture : public Aperture {
public:
	CircleAperture(int code, double d, double x, double y);

	void RenderAperture() override;
	bool SolidCircle() override;
	bool SolidRectangle() override;
};
