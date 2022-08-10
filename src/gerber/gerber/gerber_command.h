#pragma once
#include <memory>

class Aperture;

class RenderCommand {
public:
	enum Command {
		// Begin Path
		cRectangle,
		cCircle,
		cBeginLine,

		// Continue Path
		cLine,
		cArc,

		// Close Path
		cClose,

		// Render Path
		cStroke,
		cFill,
		cErase, // Used for "exposure off" aperture primitives

		// Other
		cBeginOutline, // The aperture does not matter
		cEndOutline,   // The aperture matters again
		cApertureSelect,
		cFlash
	};

	RenderCommand(Command cmd);

	Command command_;
	std::shared_ptr<Aperture> aperture_; // Used for gcApertureSelect

//protected:
	double X, Y, W, H, A; // W and H used for D, Dx and Dy; A in degrees
	std::pair<double, double> end_;
};

class ApertureSelectCommand : public RenderCommand {
public:
	ApertureSelectCommand(std::shared_ptr<Aperture> aperture);
};

class RectangleCommand : public RenderCommand {
public:
	RectangleCommand();
};

class CircleCommand : public RenderCommand {
public:
	CircleCommand(double x, double y, double diameter);
};

class BeginLineCommand : public RenderCommand {
public:
	BeginLineCommand(double x, double y);
};

class LineCommand : public RenderCommand {
public:
	LineCommand(double x, double y);
};

class ArcCommand : public RenderCommand {
public:
	ArcCommand(double x, double y, double angle);
};

class CloseCommand : public RenderCommand {
public:
	CloseCommand();
};

class StrokeCommand : public RenderCommand {
public:
	StrokeCommand();
};
class FillCommand : public RenderCommand {
public:
	FillCommand();
};

class EraseCommand : public RenderCommand {
public:
	EraseCommand();
};

class BeginOutlineCommand : public RenderCommand {
public:
	BeginOutlineCommand();
};

class EndOutlineCommand : public RenderCommand {
public:
	EndOutlineCommand();
};

class FlashCommand : public RenderCommand {
public:
	FlashCommand(double x, double y);
};
