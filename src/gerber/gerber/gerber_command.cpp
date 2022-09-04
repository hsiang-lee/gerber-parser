#include "gerber_command.h"


RenderCommand::RenderCommand(RenderCommand::Command cmd) {
	command_ = cmd;

	X = Y = W = H = A = 0.0;
	end_.first = end_.second = 0.0;
}

ApertureSelectCommand::ApertureSelectCommand(std::shared_ptr<Aperture> aperture) : RenderCommand(cApertureSelect)
{
	aperture_ = aperture;
}

RectangleCommand::RectangleCommand() : RenderCommand(cRectangle)
{
}

CircleCommand::CircleCommand(double x, double y, double diameter) : RenderCommand(cCircle)
{
	X = x;
	Y = y;
	W = diameter;
}

BeginLineCommand::BeginLineCommand(double x, double y) : RenderCommand(cBeginLine)
{
	X = x;
	Y = y;
}

LineCommand::LineCommand(double x, double y) : RenderCommand(cLine)
{
	X = x;
	Y = y;
}

ArcCommand::ArcCommand(double x, double y, double angle) : RenderCommand(cArc)
{
	X = x;
	Y = y;
	A = angle;
}

CloseCommand::CloseCommand() : RenderCommand(cClose)
{
}

StrokeCommand::StrokeCommand() : RenderCommand(cStroke)
{
}

FillCommand::FillCommand() : RenderCommand(cFill)
{
}

EraseCommand::EraseCommand() : RenderCommand(cErase)
{
}

BeginOutlineCommand::BeginOutlineCommand() : RenderCommand(cBeginOutline)
{
}

EndOutlineCommand::EndOutlineCommand() : RenderCommand(cEndOutline)
{
}

FlashCommand::FlashCommand(double x, double y) : RenderCommand(cFlash)
{
	X = x;
	Y = y;
}
