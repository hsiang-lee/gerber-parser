#pragma once
#include <string>
#include <vector>
#include <memory>
#include "parser.h"

class Gerber;
class GerberMacro;
class GerberAperture;

class ParameterParser : public Parser {
public:
	ParameterParser(Gerber& gerber);
	~ParameterParser();

	bool Run() override;
	bool EndOfFile() override;

private:
	bool ApertureDefinition();

	bool ApertureCircle(int code);
	bool ApertureRectangle(int code);
	bool ApertureObround(int code);
	bool AperturePolygon(int code);
	bool ApertureMacro(int code, const std::string& name);

	bool ApertureMacro();

	// Directive Parameters
	bool AxisSelect();
	bool FormatStatement();
	bool MirrorImage();
	bool Mode();
	bool Offset();
	bool ScaleFactor();

	// Unknown Parameters
	bool IC();

	// Aperture Parameters
	bool Add(std::shared_ptr<GerberAperture> aperture);

	// Level-Specific Parameters
	bool Knockout();
	bool LevelName();
	bool LevelPolarity();
	bool StepAndRepeat();

	// Miscellaneous Parameters
	bool IncludeFile();

	// Attributes
	bool Attribute();

	bool ImageJustify();
	bool ImageName();
	bool ImageOffset();
	bool ImagePolarity();
	bool ImageRotation();
	bool PlotFilm();

	void Add(std::shared_ptr<GerberMacro> macro);
	std::shared_ptr<GerberMacro> FindMacro(const std::string& name);

	double Get_mm(double  number);

	Gerber& gerber_;

	bool incremental_{ false };
	double offset_a_{ 0.0 }, offset_b_{ 0.0 };
	double scale_a_{ 1.0 }, scale_b_{ 1.0 };

	std::vector<std::shared_ptr<GerberMacro>> macros_;
};