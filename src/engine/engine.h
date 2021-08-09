#pragma once
#include "gerber.h"


class RenderCommand;
class GerberLevel;

class Engine {
public:
	virtual void EndDraw() = 0;
	virtual void BeginDraw(bool negative) = 0;

	virtual void BeginOutline() = 0;
	virtual void EndOutline() = 0;

	virtual void FillEvenOdd() = 0;
	virtual void Stroke() = 0;
	virtual void Close() = 0;
	virtual void DrawArc(double x, double y, double degree) = 0;
	virtual void DrawLine(double x, double y) = 0;
	virtual void BeginSolidCircleLine(double x, double y, double line_width) = 0;
	virtual void BeginLine(double x, double y) = 0;
	virtual void DrawCircle(double x, double y, double r) = 0;
	virtual void DrawRectangle(double x, double y, double w, double h) = 0;

	virtual void DrawRectLine(
		double x1, double y1, // Start
		double x2, double y2, // End
		double w, double h   // Rect Width; Height
	) = 0;

	virtual void ApertureErase(double left, double bottom, double top, double right) = 0;
	virtual void ApertureFill() = 0;
	virtual void ApertureStroke() = 0;
	virtual void ApertureClose() = 0;
	virtual void DrawApertureArc(double x, double y, double angle) = 0;
	virtual void DrawApertureLine(double x, double y) = 0;
	virtual void BeginApertureLine(double x, double y) = 0;
	virtual void DrawAperatureCircle(double x, double y, double w) = 0;
	virtual void DrawApertureRect(double x, double y, double w, double h) = 0;
	virtual void EndDrawAperture() = 0;
	virtual void PrepareDrawAperture() = 0;

	virtual void PrepareCopyLayer(double left, double bottom, double right, double top) = 0;
	virtual void CopyLayer(int count_x, int count_y, double step_x, double step_y) = 0;
	virtual void Prepare2Render() = 0;

	virtual bool PrepareExistAperture(int code) = 0;
	virtual void BeginRender() = 0;
	virtual void EndRender() = 0;

	virtual int Flash(double x, double y) = 0;
	virtual void EndDrawNewAperture(int code) = 0;
	virtual void NewAperture(double left, double bottom, double right, double top) = 0;

	bool convert_strokes2fills_{ false };
};
