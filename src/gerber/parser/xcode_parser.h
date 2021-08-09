#pragma once
#include "parser.h"

class Gerber;

class XCodeParser : public Parser {
public:
	XCodeParser(Gerber& gerber);

	bool Run() override;
	bool EndOfFile() override;

private:
	Gerber& gerber_;
};