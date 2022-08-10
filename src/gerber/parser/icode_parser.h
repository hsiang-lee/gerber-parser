#pragma once
#include "parser.h"

class GerberParser;

class ICodeParser : public Parser {
public:
	ICodeParser(GerberParser& gerber);

	bool Run() override;
	bool EndOfFile() override;

private:
	GerberParser& gerber_parser_;
};