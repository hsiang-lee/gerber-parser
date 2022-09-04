#pragma once
#include "pparser.h"
#include <string>
#include <memory>


class GerberParser;
class ParameterParser;

class OfParser : public PParser {
public:
	OfParser(GerberParser& gerber, ParameterParser& param_parser);
	bool Run() override;

private:
	GerberParser& gerber_parser_;
	ParameterParser& param_parser_;

	double offset_a_{ 0.0 }, offset_b_{ 0.0 };
};
