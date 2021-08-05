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

#include "pdfString.h"


using namespace std;


pdfString::pdfString() {
}


pdfString::~pdfString() {
}


void pdfString::Set(const char* String) {
	int j;
	for (j = 0; String[j]; j++);
	Set(String, j);
}


void pdfString::Set(const char* String, unsigned Length) {
	unsigned n;
	uint8_t  c;

	Value.clear();

	for (n = 0; n < Length; n++) {
		c = String[n];
		switch (c) {
		case '(': Value.append("\\("); break;
		case ')': Value.append("\\)"); break;
		case '\\': Value.append("\\\\"); break;
		case '\n': Value.append("\\n"); break;
		case '\r': Value.append("\\r"); break;
		case '\t': Value.append("\\t"); break;
		case '\b': Value.append("\\b"); break;
		case '\f': Value.append("\\f"); break;
		default: Value.append(1, (char)c); break;
		}
	}
}


void pdfString::Set(
	int Year,
	int Month,
	int Day,
	int Hour,
	int Minute,
	int Second
) {
	char s[0x100];

	if (Month > 0) {
		if (Day > 0) {
			sprintf(s, "D:%04d%02d%02d%02d%02d%02d",
				Year, Month, Day, Hour, Minute, Second);
		}
		else {
			sprintf(s, "D:%04d%02d", Year, Month);
		}
	}
	else {
		sprintf(s, "D:%04d", Year);
	}

	Value.assign(s);
}


void pdfString::SetUnicode(const char* String) {
	bool HasUnicode = false;

	u16string UTF16 = UTF_Converter.UTF16(String);

	string UTF16_BE;

	UTF16_BE.append(1, (char)((uint8_t)254));
	UTF16_BE.append(1, (char)((uint8_t)255));

	for (int n = 0; UTF16[n]; n++) {
		if (UTF16[n] >= 0x80) HasUnicode = true;
		UTF16_BE.append(1, (char)((uint8_t)(UTF16[n] >> 8)));
		UTF16_BE.append(1, (char)((uint8_t)(UTF16[n] & 0xFF)));
	}

	if (HasUnicode) Set(UTF16_BE.c_str(), UTF16_BE.length());
	else           Set(String);
}


bool pdfString::Empty() {
	return Value.empty();
}



int pdfString::GetLength() {
	return Value.length() + 2;
}


int pdfString::GetOutput(char* Buffer) {
	unsigned n;

	Buffer[0] = '(';
	for (n = 0; n < Value.length(); n++) Buffer[n + 1] = Value[n];
	Buffer[n + 1] = ')';

	return n + 2;
}


