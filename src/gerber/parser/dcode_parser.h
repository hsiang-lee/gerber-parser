#pragma once
#include "parser.h"

class Gerber;

class DCodeParser : public Parser {
public:
	DCodeParser(Gerber& gerber);

	bool Run() override;

private:
	Gerber& gerber_;
};