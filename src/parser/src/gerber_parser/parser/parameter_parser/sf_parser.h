#pragma once
#include "pparser.h"
#include <string>


class GerberParserImpl;
class ParameterParser;

class SfParser : public PParser {
public:
	SfParser(GerberParserImpl& gerber, ParameterParser& param_parser);
	bool Run() override;

private:
	GerberParserImpl& gerber_parser_;
	ParameterParser& param_parser_;
	double scale_a_{ 1.0 }, scale_b_{ 1.0 };
};
