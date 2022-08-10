#pragma once

#include <vector>
#include <memory>

#include "bound_box.h"
#include "gerber_command.h"

#include <QPainterPath>

class GerberLevel;
class Primitive;

class ApertureLevel {
public:
	BoundBox bound_box_;
	std::vector<std::shared_ptr<RenderCommand>> render_commands_{};

	void NewPrimitime();
	std::unique_ptr<Primitive> primitive_;
	std::vector<std::unique_ptr<Primitive>> primitives_;

public:
	ApertureLevel(const std::vector<std::shared_ptr<RenderCommand>>& renders, const BoundBox& box);

	void Draw();
	void Erase(const BoundBox& box);
	void Fill();
	void Stroke();
	void Close();
	void DrawLine(double x, double y);
	void BeginLine(double x, double y);
	void DrawCircle(double x, double y, double w);
	void DrawRect(double x, double y, double w, double h);

	void DrawArcScaled(double x, double y, double degree);
	void DrawArc(double x, double y, double degree);
};
