#pragma once
#include "gerber.h"

class Engine;
class GerberLevel;
class RenderCommand;
class GerberAperture;

class GerberRender {
public:
	GerberRender(Engine* engine);
	~GerberRender();

	int RenderGerber(std::shared_ptr<Gerber>);

private:
	int Draw(std::shared_ptr<RenderCommand> render);

	void DrawAperture(
		std::vector<std::shared_ptr<RenderCommand>> renders,
		double left,
		double bottom,
		double right,
		double top
	);

	int RenderLayer(std::shared_ptr<GerberLevel> level);
	int RenderLevel(std::shared_ptr<GerberLevel> level);

	Engine* engine_;

	bool outline_path_{ false };
	bool solid_circle_{ false };
	bool solid_rectangle_{ false };

	double line_width_{ 0.0 };

	double rect_w_{ 0 };
	double rect_h_{ 0 };
	double rect_x_{ 0 };
	double rect_y_{ 0 };

	std::shared_ptr<Gerber> gerber_;
};