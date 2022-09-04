#pragma once
#include "parser.h"


class GerberParser;

class NCodeParser : public Parser {
public:
	NCodeParser(GerberParser& gerber);

	bool Run() override;
	bool EndOfFile() override;

private:
	GerberParser& gerber_parser_;
};