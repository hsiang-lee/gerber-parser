#pragma once
#include "parser.h"


class Gerber;

class GCodeParser : public Parser {
public:
	GCodeParser(Gerber& gerber);

	bool Run() override;
	bool EndOfFile() override;

private:
	Gerber& gerber_;
};