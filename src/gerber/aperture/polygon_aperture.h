#pragma once
#include "aperture.h"


class PolygonAperture : public Aperture {
public:
	PolygonAperture(int code, double w, int n, double a, double x, double y); // Width; Side count; Rotation

	void RenderAperture() override;
	bool SolidCircle() override;
	bool SolidRectangle() override;

private:
	int side_count_; // Number of sides
};
