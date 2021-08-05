#pragma once
#include <memory>

class GerberAperture;

class RenderCommand {
public:
	enum GerberCommand {
		// Begin Path
		gcRectangle,
		gcCircle,
		gcBeginLine,

		// Continue Path
		gcLine,
		gcArc,

		// Close Path
		gcClose,

		// Render Path
		gcStroke,
		gcFill,
		gcErase, // Used for "exposure off" aperture primitives

		// Other
		gcBeginOutline, // The aperture does not matter
		gcEndOutline,   // The aperture matters again
		gcApertureSelect,
		gcFlash
	};


	RenderCommand(GerberCommand cmd);

	GerberCommand command_;

	double X, Y, W, H, A; // W and H used for D, Dx and Dy; A in degrees
	struct {
		double X, Y;
	} End;

	std::shared_ptr<GerberAperture> aperture_; // Used for gcApertureSelect
};
