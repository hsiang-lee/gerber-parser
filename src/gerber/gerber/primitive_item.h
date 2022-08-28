#pragma once
#include <vector>
#include <memory>


class GerberMacro;
class OperatorItem;

class PrimitiveItem {
public:
	enum Type {
		kComment = 0,
		kCircle = 1,
		kLineVector = 2,
		kLineVector2 = 20,
		kLineCenter = 21,
		kLineLowerLeft = 22,
		kEndOfFile = 3,
		kOutline = 4,
		kPolygon = 5,
		kMoire = 6,
		kThermal = 7,
		kAssignment = -1,
	};

	Type type_;
	std::vector<std::shared_ptr<OperatorItem>> modifiers_;
	int index_;

	PrimitiveItem();
	~PrimitiveItem();

	bool Render(GerberMacro& macro);

private:
	bool RenderCircle(GerberMacro& macro);
	bool RenderLineVector(GerberMacro& macro);
	bool RenderLineCenter(GerberMacro& macro);
	bool RenderLineLowerLeft(GerberMacro& macro);
	bool RenderOutline(GerberMacro& macro);
	bool RenderPolygon(GerberMacro& macro);
	bool RenderMoire(GerberMacro& macro);
	bool RenderThermal(GerberMacro& macro);
	bool RenderAssignment(GerberMacro& macro);
	void RenderLine(
		double x1, double y1,
		double x2, double y2,
		double x3, double y3,
		double x4, double y4,
		double x_r, double y_r, // Rotation Center
		double angle, GerberMacro& macro
	);
	double Evaluate(const std::shared_ptr<OperatorItem>& root, const std::vector<double>& modifiers);

	static constexpr double kPi = 3.141592653589793238463;
};
