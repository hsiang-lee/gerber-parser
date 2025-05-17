#pragma once
#include "pparser.h"
#include <string>


class GerberParserImpl;
class ParameterParser;

class MiParser : public PParser {
public:
	MiParser(GerberParserImpl& gerber, ParameterParser& param_parser);
	bool Run() override;

private:
	GerberParserImpl& gerber_parser_;
	ParameterParser& param_parser_;
};
