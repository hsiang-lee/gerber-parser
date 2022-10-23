#pragma once
#include <vector>
#include <string>

class GerberApi GerberFile {
public:
	GerberFile(const std::vector<char>& data);

	std::vector<char> buffer_;

	unsigned pointer_{ 0 };
	unsigned line_number_{ 1 };

	bool EndOfFile();
	bool MoreThanLastOne() const;
	bool SkipWhiteSpace();

	char GetChar();
	char PeekChar();
	char PeekNextChar();

	bool QueryCharUntilNotWhiteSpace(char c);
	bool QueryCharUntilEnd(char c);

	bool GetString(std::string& str);

	bool GetInteger(int& integer);
	bool GetFloat(double& number);
	bool GetCoordinate(double& number, int integer, int decimal, bool omit_trailing_zeroes);

private:
	static bool IsNumber(char cur_char);
	bool GetSign();
};
