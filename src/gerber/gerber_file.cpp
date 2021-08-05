#include "gerber_file.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <glog/logging.h>


bool GerberFile::Load(const std::string& file_name)
{
	std::ifstream file(file_name, std::ios::in);
	if (!file) {
		std::cout << "failed to open gerber file." << std::endl;
		return false;
	}

	buffer_ = { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
	index_ = 0;
	line_number_ = 0;

	return true;
}

bool GerberFile::EndOfFile()
{
	return index_ >= buffer_.size();
}

bool GerberFile::SkipWhiteSpace() {
	while (!EndOfFile()) {
		switch (buffer_[index_]) {
		case '\n':
			line_number_++;
		case ' ':
		case '\t':
		case '\r':
			index_++;
			break;

		default:
			return false;
		}
	}

	return true;
}

char GerberFile::GetChar()
{
	return buffer_[index_++];
}

char GerberFile::PeekChar() {
	return buffer_[index_];
}

char GerberFile::PeekNextChar() {
	return buffer_[index_ + 1];
}

bool GerberFile::QueryCharUntilNotWhiteSpace(char c)
{
	if (SkipWhiteSpace() || PeekChar() != c) {
		return false;
	}

	++index_;
	return true;
}

bool GerberFile::QueryCharUntilEnd(char c)
{
	while (!EndOfFile()) {
		if (PeekChar() == c) {
			return true;
		}

		++index_;
	}

	return false;
}

bool GerberFile::GetInteger(int& integer) {
	bool     sign = false;
	unsigned i = index_;

	SkipWhiteSpace();

	integer = 0;

	if (index_ < buffer_.size() && buffer_[index_] == '-') {
		sign = true;
		index_++;
	}
	else if (index_ < buffer_.size() && buffer_[index_] == '+') {
		index_++;
	}

	while (index_ < buffer_.size()) {
		if (buffer_[index_] >= '0' && buffer_[index_] <= '9') {
			integer *= 10;
			integer += buffer_[index_] - '0';
			index_++;
		}
		else {
			if (sign) integer *= -1;
			return (index_ > i);
		}
	}

	index_ = i;
	return false;
}

bool GerberFile::GetFloat(double& number) {
	int       integer = 0;
	bool      sign = false;
	double    scale = 0.1;
	unsigned i = index_;

	SkipWhiteSpace();

	if (index_ < buffer_.size() && buffer_[index_] == '-') {
		sign = true;
		index_++;
	}
	else if (index_ < buffer_.size() && buffer_[index_] == '+') {
		index_++;
	}

	while (index_ < buffer_.size()) {
		if (buffer_[index_] >= '0' && buffer_[index_] <= '9') {
			integer *= 10;
			integer += buffer_[index_] - '0';
			index_++;
		}
		else {
			break;
		}
	}

	number = integer;

	if (index_ < buffer_.size() && buffer_[index_] == '.') {
		index_++;
		while (index_ < buffer_.size()) {
			if (buffer_[index_] >= '0' && buffer_[index_] <= '9') {
				number += (buffer_[index_] - '0') * scale;
				scale *= 0.1;
				index_++;
			}
			else {
				break;
			}
		}
	}

	if (index_ < buffer_.size()) {
		if (sign)
			number *= -1.0;

		return (index_ > i);
	}

	return false;
}

bool GerberFile::GetCoordinate(double& number, int integer, int decimal, bool omit_trailing_zeroes) {
	int      j;
	int      n = 0;
	bool     sign = false;
	unsigned i = index_;

	SkipWhiteSpace();

	number = 0;

	if (!EndOfFile() && buffer_[index_] == '-') {
		sign = true;
		index_++;
	}
	else if (!EndOfFile() && buffer_[index_] == '+') {
		index_++;
	}

	while (!EndOfFile()) {
		if (buffer_[index_] >= '0' && buffer_[index_] <= '9') {
			number *= 10;
			number += buffer_[index_] - '0';
			index_++;
			n++;
		}
		else if (buffer_[index_] == '.') {
			index_ = i;
			return GetFloat(number);
		}
		else {
			if (sign) number *= -1;
			if (omit_trailing_zeroes) {
				for (j = 0; j < (integer + decimal - n); j++) {
					number *= 10;
				}
			}
			for (j = 0; j < decimal; j++) number /= 10;
			if (!n) {
				LOG(WARNING) << "Line " << line_number_ << " - Warning: Ignoring ill-formed coordinate";
			}
			return true;
		}
	}

	return false;
}

bool GerberFile::GetString(std::string& str) {
	while (!EndOfFile()) {
		SkipWhiteSpace();

		switch (PeekChar()) {
		case 0:
			LOG(ERROR) << "Line " << line_number_ << " - Error: Null in name not allowed";
			return false;
		case '*':
		case ',':
			return true;
		default:
			str.push_back(GetChar());
			break;
		}
	}

	return false;
}
