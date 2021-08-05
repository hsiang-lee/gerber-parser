//==============================================================================
// Copyright (C) John-Philip Taylor
// jpt13653903@gmail.com
// This file is part of a library
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
//==============================================================================

#include "afmParser.h"


afmParser::afmParser() {
	buffer_ = 0;
	Index = 0;
	Length = 0;
}


afmParser::~afmParser() {
	if (buffer_) delete[] buffer_;
}


bool afmParser::Open(const char* FileName) {
	bool b;

	if (File.Open(FileName, FILE_WRAPPER::faRead)) {
		Index = 0;
		Length = File.GetSize();
		buffer_ = new char[Length];
		b = (File.Read(buffer_, Length) == Length);
		File.Close();
		return b;

	}
	else {
		error("%s", GetErrorString(GetLastError()));
	}

	return false;
}


void afmParser::Close() {
	if (buffer_) delete[] buffer_;
	buffer_ = 0;
	Index = 0;
	Length = 0;
}


int afmParser::Compare(const char* s1, const char* s2) {
	int j;

	for (j = 0; s1[j] && s2[j]; j++) {
		if (s1[j] < s2[j]) return -1;
		if (s1[j] > s2[j]) return  1;
	}

	if (s1[j]) return  1;
	if (s2[j]) return -1;

	return 0;
}


bool afmParser::GetEndOfLine() {
	GetWhiteSpace();

	if (Index < Length && buffer_[Index] == '\n') {
		Index++;
		return true;
	}

	if (Index < Length - 1 && buffer_[Index] == '\r' && buffer_[Index + 1] == '\n') {
		Index += 2;
		return true;
	}

	return false;
}


void afmParser::GetWhiteSpace() {
	while (Index < Length) {
		if (
			buffer_[Index] != ' ' &&
			buffer_[Index] != '\t'
			) return;
		Index++;
	}
}


char* afmParser::GetLine() {
	unsigned j = 0;
	unsigned f = Index;
	char* Line = 0;

	while (f < Length && buffer_[f] != '\r' && buffer_[f] != '\n') f++;

	Line = new char[f - Index + 1];
	while (Index < f) {
		Line[j++] = buffer_[Index++];
	}
	Line[j] = 0;

	GetEndOfLine();

	return Line;
}


bool afmParser::FindEnd(char* Key) {
	int  j;
	char String[0x100];

	Key[0] = 'E';
	Key[1] = 'n';
	Key[2] = 'd';

	for (j = 5; Key[j]; j++) {
		Key[j - 2] = Key[j];
	}
	Key[j - 2] = 0;

	while (GetKey(String)) {
		if (!Compare(String, Key)) {
			GetEndOfLine();
			return true;
		}
		while (Index < Length && buffer_[Index] != '\n') Index++;
		Index++;
	}

	return false;
}


bool afmParser::GetKey(char* Key) {
	int      j = 0;
	unsigned i = Index;

	while (GetEndOfLine());

	while (Index < Length) {
		switch (buffer_[Index]) {
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			Key[j] = 0;
			return (j > 0);

		default:
			break;
		}
		Key[j++] = buffer_[Index];
		Index++;
	}

	Index = i;
	return false;
}


bool afmParser::GetString(char* String) {
	int      j = 0;
	unsigned i = Index;

	GetWhiteSpace();

	while (Index < Length) {
		switch (buffer_[Index]) {
		case '\r':
		case '\n':
			String[j] = 0;
			return (j > 0);

		default:
			break;
		}
		String[j++] = buffer_[Index];
		Index++;
	}

	Index = i;
	return false;
}


bool afmParser::GetName(char* Name) {
	int      j = 0;
	unsigned i = Index;

	GetWhiteSpace();

	while (Index < Length) {
		switch (buffer_[Index]) {
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			Name[j] = 0;
			return (j > 0);

		default:
			break;
		}
		Name[j++] = buffer_[Index];
		Index++;
	}

	Index = i;
	return false;
}


bool afmParser::GetNumber(double* Number) {
	bool     negative_ = false;
	double   Integer = 0.0;
	double   Fraction = 0.0;
	double   Scale = 0.1;
	unsigned i = Index;

	GetWhiteSpace();

	if (Index < Length && buffer_[Index] == '-') {
		Index++;
		negative_ = true;
	}
	else if (Index < Length && buffer_[Index] == '+') {
		Index++;
	}

	while (Index < Length) {
		if (buffer_[Index] == '.') {
			Index++;
			while (Index < Length) {
				if (buffer_[Index] < '0' || buffer_[Index] > '9') {
					if (Index == i + 1) {
						Index = i;
						return false;
					}
					*Number = Integer + Fraction;
					if (negative_) *Number *= -1.0;
					return true;
				}
				Fraction += (buffer_[Index] - '0') * Scale;
				Scale /= 10.0;
				Index++;
			}
		}

		if (buffer_[Index] < '0' || buffer_[Index] > '9') {
			if (Index == i) return false;
			*Number = Integer;
			if (negative_) *Number *= -1.0;
			return true;
		}
		Integer *= 10.0;
		Integer += (buffer_[Index] - '0');
		Index++;
	}

	Index = i;
	return false;
}


bool afmParser::GetHexInteger(unsigned* Integer) {
	unsigned t = 0;
	unsigned i = Index;

	GetWhiteSpace();

	if (Index > Length) return false;
	if (buffer_[Index] != '<') return false;
	Index++;

	while (Index < Length) {
		if (buffer_[Index] >= '0' && buffer_[Index] <= '9') {
			t *= 0x10;
			t += (buffer_[Index] - '0');
			Index++;

		}
		else if (buffer_[Index] >= 'a' && buffer_[Index] <= 'f') {
			t *= 0x10;
			t += (buffer_[Index] - 'a' + 10);
			Index++;

		}
		else if (buffer_[Index] >= 'A' && buffer_[Index] <= 'F') {
			t *= 0x10;
			t += (buffer_[Index] - 'A' + 10);
			Index++;

		}
		else {
			if (Index == i) return false;
			if (buffer_[Index] != '>') {
				Index = i;
				return false;
			}
			Index++;
			*Integer = t;
			return true;
		}
	}

	Index = i;
	return false;
}


bool afmParser::GetInteger(int* Integer) {
	bool     negative_ = false;
	int      t = 0;
	unsigned i = Index;

	GetWhiteSpace();

	if (Index < Length && buffer_[Index] == '-') {
		Index++;
		negative_ = true;
	}
	else if (Index < Length && buffer_[Index] == '+') {
		Index++;
	}

	if (buffer_[Index] == '<') {
		unsigned u;
		if (!GetHexInteger(&u)) return false;
		*Integer = u;
		return true;
	}

	while (Index < Length) {
		if (buffer_[Index] < '0' || buffer_[Index] > '9') {
			if (Index == i) return false;
			*Integer = t;
			if (negative_) *Integer *= -1.0;
			return true;
		}
		t *= 10;
		t += (buffer_[Index] - '0');
		Index++;
	}

	Index = i;
	return false;
}


bool afmParser::GetBoolean(bool* Boolean) {
	unsigned i = Index;

	GetWhiteSpace();

	if (Index < Length - 3 &&
		buffer_[Index] == 't' &&
		buffer_[Index + 1] == 'r' &&
		buffer_[Index + 2] == 'u' &&
		buffer_[Index + 3] == 'e') {
		*Boolean = true;
		return true;
	}

	if (Index < Length - 4 &&
		buffer_[Index] == 'f' &&
		buffer_[Index + 1] == 'a' &&
		buffer_[Index + 2] == 'l' &&
		buffer_[Index + 3] == 's' &&
		buffer_[Index + 4] == 'e') {
		*Boolean = false;
		return true;
	}

	Index = i;
	return false;
}


bool afmParser::Ignore(char* Key) {
	char c;

	c = Key[5];
	Key[5] = 0;
	if (!Compare(Key, "Start")) {
		Key[5] = c;
		if (!FindEnd(Key)) return false;
	}
	else {
		while (Index < Length && buffer_[Index] != '\n') Index++;
		Index++;
	}

	return true;
}


bool afmParser::IgnoreCharSection() {
	while (Index < Length) {
		if (buffer_[Index] == ';') {
			Index++;
			break;

		}
		else if (buffer_[Index] == '\n') {
			break;
		}
		Index++;
	}

	if (Index < Length) return true;

	return false;
}

