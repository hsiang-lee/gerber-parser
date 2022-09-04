#pragma once
#include "parser.h"

class GerberParser;

class YCodeParser : public Parser {
public:
	YCodeParser(GerberParser& gerber);

	bool Run() override;
	bool EndOfFile() override;

private:
	GerberParser& gerber_parser_;
};