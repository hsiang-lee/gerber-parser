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

// A variable length array that manages its own memory

#ifndef PDFSTRING_H
#define PDFSTRING_H


#include "pdfObject.h"
#include "tools/UTF_Converter.h"


class pdfString : public pdfObject {
private:
	std::string Value;

public:
	void Set(const char* String); // null-terminated
	void Set(const char* String, unsigned Length);
	void Set(
		int Year,
		int Month = 0,
		int Day = 0,
		int Hour = 0,
		int Minute = 0,
		int Second = 0
	);
	void SetUnicode(const char* String); // UTF-8

	pdfString(); // Initialise the memory to a 1-length array with null value
	~pdfString(); // Delete the memory

	bool Empty();
	int  GetLength();
	int  GetOutput(char* Buffer);
};


#endif


