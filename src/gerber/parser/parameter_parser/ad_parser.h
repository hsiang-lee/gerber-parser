#pragma once
#include "pparser.h"
#include <string>
#include <memory>


class GerberParser;
class Aperture;
class ParameterParser;

class AdParser : public PParser {
public:
	AdParser(GerberParser& gerber, ParameterParser& param_parser);
	bool Run() override;

private:
	bool ApertureMacro(int code, const std::string& name);
	bool ApertureCircle(int code);
	bool ApertureRectangle(int code);
	bool ApertureObround(int code);
	bool AperturePolygon(int code);

	bool Add(const std::shared_ptr<Aperture>& aperture);

	GerberParser& gerber_parser_;
	ParameterParser& param_parser_;
};
