#pragma once
#include "parser.h"

class GerberParser;

class XCodeParser : public Parser {
public:
	XCodeParser(GerberParser& gerber);

	bool Run() override;
	bool EndOfFile() override;

private:
	GerberParser& gerber_parser_;
};
