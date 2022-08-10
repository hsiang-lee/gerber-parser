#pragma once
#include "parser.h"

class GerberParser;

class MCodeParser : public Parser {
public:
	MCodeParser(GerberParser& gerber);

	bool Run() override;
	bool EndOfFile() override;

private:
	GerberParser& gerber_parser_;
	bool end_of_file_{ false };
};