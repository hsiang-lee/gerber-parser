#pragma once
#include "parser.h"


class GerberParserImpl;

class NCodeParser : public Parser {
public:
	NCodeParser(GerberParserImpl& gerber);

	bool Run() override;
	bool EndOfFile() override;

private:
	GerberParserImpl& gerber_parser_;
};