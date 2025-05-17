#pragma once
#include "pparser.h"
#include <string>


class GerberParserImpl;
class ParameterParser;

class IoParser : public PParser {
public:
	IoParser(GerberParserImpl& gerber, ParameterParser& param_parser);
	bool Run() override;

private:
	GerberParserImpl& gerber_parser_;
	ParameterParser& param_parser_;
};
