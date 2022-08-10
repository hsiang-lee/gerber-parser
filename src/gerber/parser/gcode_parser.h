#pragma once
#include "parser.h"


class GerberParser;

class GCodeParser : public Parser {
public:
	GCodeParser(GerberParser& gerber);

	bool Run() override;
	bool EndOfFile() override;

private:
	GerberParser& gerber_parser_;
};