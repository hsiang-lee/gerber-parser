#pragma once
#include "pparser.h"
#include <string>
#include <memory>


class GerberParser;
class ParameterParser;

class SfParser : public PParser {
public:
	SfParser(GerberParser& gerber, ParameterParser& param_parser);
	bool Run() override;

private:
	GerberParser& gerber_parser_;
	ParameterParser& param_parser_;
	double scale_a_{ 1.0 }, scale_b_{ 1.0 };
};
