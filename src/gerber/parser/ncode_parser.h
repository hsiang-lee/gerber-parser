#pragma once
#include "parser.h"


class Gerber;

class NCodeParser : public Parser {
public:
	NCodeParser(Gerber& gerber);

	bool Run() override;
	bool EndOfFile() override;

private:
	Gerber& gerber_;
};