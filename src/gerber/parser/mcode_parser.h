#pragma once
#include "parser.h"

class Gerber;

class MCodeParser : public Parser {
public:
	MCodeParser(Gerber& gerber);

	bool Run() override;
	bool EndOfFile() override;

private:
	Gerber& gerber_;
	bool end_of_file_{ false };
};