#pragma once
#include "parser.h"

class Gerber;

class YCodeParser : public Parser {
public:
	YCodeParser(Gerber& gerber);

	bool Run() override;
	bool EndOfFile() override;

private:
	Gerber& gerber_;
};