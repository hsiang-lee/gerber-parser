#pragma once

#include <math.h>
#include <vector>

#include "gerber_macro.h"
#include "gerber_command.h"


class GerberMacro;
class RenderCommand;
class GerberAperture {
private:
	enum TYPE {
		tCircle,
		tRectangle,
		tObround,
		tPolygon,
		tMacro
	};

	TYPE type_;

	std::vector<std::shared_ptr<RenderCommand>> render_commands_;

	void Add(std::shared_ptr<RenderCommand> render);

	void RenderHole();
	void RenderObround();
	void RenderCircle();
	void RenderPolygon();
	void RenderRectangle();

	void RenderAperture();

	// Standard Aperture (not Custom) modifiers
	double dimension_x_; // Also used for outside diameter of circles
	double dimension_y_;
	double hole_x_; // For no hole, make this negative
	double hole_y_; // For a round hole make this negative

	// Regular Polygon additional modifiers
	int side_count_; // Number of sides
	double rotation_;  // Degrees of rotaion (rotate the whole thing CCW)

public:
	GerberAperture();
	~GerberAperture();

	// Image bounding box
	double left_;
	double bottom_;
	double right_;
	double top_;

	int  code_; // The code to use in the D-Code to load this tool

	// Standard Aperture Generators
	void Circle(double d);
	void Rectangle(double w, double h);
	void Obround(double w, double h);
	void Polygon(double w, int    n, double a); // Width; Side count; Rotation

	// Holes
	void HoleCircle(double d);
	void HoleRectangle(double w, double h);

	// ApertureMacro: Modifiers is a null-terminated array
	// The user must free Macro
	// Modifiers is copied, so the original can be reused for something else
	void UseMacro(std::shared_ptr<GerberMacro> macro, double* modifiers, int modifier_count);

	// Used to determine if it is a basic shape or not
	bool SolidCircle();
	bool SolidRectangle();

	// Linked list of render commands
	// Memory freed automatically
	std::vector<std::shared_ptr<RenderCommand>> Render();
};

