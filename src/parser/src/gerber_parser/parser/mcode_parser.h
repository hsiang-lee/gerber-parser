#pragma once
#include "parser.h"

class GerberParserImpl;

class MCodeParser : public Parser {
public:
	MCodeParser(GerberParserImpl& gerber);

	bool Run() override;
	bool EndOfFile() override;

private:
	GerberParserImpl& gerber_parser_;
	bool end_of_file_{ false };
};