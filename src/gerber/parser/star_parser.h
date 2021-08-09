#pragma once
#include "parser.h"

class Gerber;

class StarParser : public Parser {
public:
	StarParser(Gerber& gerber);

	bool Run() override;
	bool EndOfFile() override;

private:
	Gerber& gerber_;
};