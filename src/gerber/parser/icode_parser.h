#pragma once
#include "parser.h"

class Gerber;

class ICodeParser : public Parser {
public:
	ICodeParser(Gerber& gerber);

	bool Run() override;
	bool EndOfFile() override;

private:
	Gerber& gerber_;
};