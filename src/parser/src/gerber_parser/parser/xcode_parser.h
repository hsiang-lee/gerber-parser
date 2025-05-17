#pragma once
#include "parser.h"

class GerberParserImpl;

class XCodeParser : public Parser {
public:
	XCodeParser(GerberParserImpl& gerber);

	bool Run() override;
	bool EndOfFile() override;

private:
	GerberParserImpl& gerber_parser_;
};
