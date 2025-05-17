#pragma once
#include "parser.h"

class GerberParserImpl;

class YCodeParser : public Parser {
public:
	YCodeParser(GerberParserImpl& gerber);

	bool Run() override;
	bool EndOfFile() override;

private:
	GerberParserImpl& gerber_parser_;
};