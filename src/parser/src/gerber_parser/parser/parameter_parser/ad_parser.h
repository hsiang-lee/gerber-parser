#pragma once
#include "pparser.h"
#include <string>

class Aperture;
class GerberParserImpl;
class ParameterParser;

class AdParser : public PParser
{
public:
    AdParser(GerberParserImpl &gerber, ParameterParser &param_parser);
    bool Run() override;

private:
    bool ApertureMacro(int code, const std::string &name);
    bool ApertureCircle(int code);
    bool ApertureRectangle(int code);
    bool ApertureObround(int code);
    bool AperturePolygon(int code);

    bool Add(Aperture *aperture);

    GerberParserImpl &gerber_parser_;
    ParameterParser &param_parser_;
};
