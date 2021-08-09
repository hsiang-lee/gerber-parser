#pragma once
#include "parser.h"

class Gerber;

class JCodeParser : public Parser {
public:
	JCodeParser(Gerber& gerber);

	bool Run() override;
	bool EndOfFile() override;

private:
	Gerber& gerber_;
};