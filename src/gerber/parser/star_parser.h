#pragma once
#include "parser.h"

class GerberParser;

class StarParser : public Parser {
public:
	StarParser(GerberParser& gerber);

	bool Run() override;
	bool EndOfFile() override;

private:
	GerberParser& gerber_parser_;
};
