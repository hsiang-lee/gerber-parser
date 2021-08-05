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

#include "pdfName.h"

pdfName::pdfName() {
	Value = 0;
}

pdfName::~pdfName() {
	if (Value) {
		delete[] Value;
	}
}

void pdfName::Set(const char* Value) {
	int j, q;

	unsigned char* val = (unsigned char*)Value;

	for (j = 0; Value[j]; j++) {
		if (
			val[j] < '!' || // 0x21
			val[j]  > '~' || // 0x7E
			val[j] == '(' ||
			val[j] == ')' ||
			val[j] == '<' ||
			val[j] == '>' ||
			val[j] == '[' ||
			val[j] == ']' ||
			val[j] == '{' ||
			val[j] == '}' ||
			val[j] == '/' ||
			val[j] == '%' ||
			val[j] == '#'
			) j += 2;
	}

	if (pdfName::Value) delete[] pdfName::Value;
	pdfName::Value = new unsigned char[j + 1];

	j = 0;
	q = 0;
	while (val[j]) {
		if (
			val[j] < '!' || // 0x21
			val[j]  > '~' || // 0x7E
			val[j] == '(' ||
			val[j] == ')' ||
			val[j] == '<' ||
			val[j] == '>' ||
			val[j] == '[' ||
			val[j] == ']' ||
			val[j] == '{' ||
			val[j] == '}' ||
			val[j] == '/' ||
			val[j] == '%' ||
			val[j] == '#'
			) {
			pdfName::Value[q] = '#';
			q++;
			pdfName::Value[q] = (val[j] >> 4) + '0';
			if (pdfName::Value[q] > '9') pdfName::Value[q] += 'A' - '0' - 10;
			q++;
			pdfName::Value[q] = (val[j] & 0x0F) + '0';
			if (pdfName::Value[q] > '9') pdfName::Value[q] += 'A' - '0' - 10;

		}
		else {
			pdfName::Value[q] = val[j];
		}
		q++;
		j++;
	}
	pdfName::Value[q] = 0;
}

char* pdfName::Get() {
	return (char*)Value;
}

bool pdfName::Empty() {
	if (!Value) return true;
	if (Value[0]) return false;
	return true;
}

bool pdfName::Equal(const char* String) {
	if (!Value) return false;
	if (!String) return false;

	int j;

	for (j = 0; String[j] && Value[j]; j++) {
		if (String[j] != Value[j]) return false;
	}
	if (String[j] != Value[j]) return false;

	return true;
}

bool pdfName::Equivalent(const char* String) {
	if (!Value) return false;
	if (!String) return false;

	pdfName Sanitised;
	Sanitised.Set(String);
	return Equal(Sanitised.Get());
}

int pdfName::GetLength() {
	if (!Value) return 0;

	int r = 0;
	while (Value[r]) r++;
	return r + 1;
}

int pdfName::GetOutput(char* Buffer) {
	if (!Value) return 0;

	int i = 0;

	Buffer[0] = '/';
	while (Value[i]) {
		Buffer[i + 1] = Value[i];
		i++;
	}

	return i + 1;
}

