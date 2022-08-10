#pragma once
#include "parser.h"

class GerberParser;

class DCodeParser : public Parser {
public:
	DCodeParser(GerberParser& gerber);

	bool Run() override;
	bool EndOfFile() override;

private:
	GerberParser& gerber_parser_;
};