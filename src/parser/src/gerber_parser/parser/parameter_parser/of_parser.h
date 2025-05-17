#pragma once
#include "pparser.h"
#include <string>


class GerberParserImpl;
class ParameterParser;

class OfParser : public PParser {
public:
	OfParser(GerberParserImpl& gerber, ParameterParser& param_parser);
	bool Run() override;

private:
	GerberParserImpl& gerber_parser_;
	ParameterParser& param_parser_;

	double offset_a_{ 0.0 }, offset_b_{ 0.0 };
};
