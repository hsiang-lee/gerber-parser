#pragma once
#include "parser.h"

class GerberParser;

class JCodeParser : public Parser {
public:
	JCodeParser(GerberParser& gerber);

	bool Run() override;
	bool EndOfFile() override;

private:
	GerberParser& gerber_parser_;
};