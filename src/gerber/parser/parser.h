#pragma once

class Parser {
public:
	virtual bool Run() = 0;
	virtual bool EndOfFile() = 0;
};
