#pragma once
#include "parser.h"

class GerberParserImpl;

class StarParser : public Parser {
public:
	StarParser(GerberParserImpl& gerber);

	bool Run() override;
	bool EndOfFile() override;

private:
	GerberParserImpl& gerber_parser_;
};
