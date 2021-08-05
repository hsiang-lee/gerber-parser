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

#ifndef AFMPARSER_H
#define AFMPARSER_H


#include "tools/file_wrapper.h"


class afmParser {
private:
	FILE_WRAPPER File;
	char* buffer_;
	unsigned     Index;
	unsigned     Length;

	bool FindEnd(char* Key);

	int  Compare(const char* s1, const char* s2);

public:
	afmParser();
	~afmParser();

	bool Open(const char* FileName);
	void Close();

	bool  GetEndOfLine();
	void  GetWhiteSpace(); // space; newline; tab
	char* GetLine(); // Returns a string, caller must free it, excludes EOL

	bool GetKey(char* Key);
	bool GetString(char* String); // Terminated by end-of-line
	bool GetName(char* Name);   // Terminated by whitespace
	bool GetNumber(double* Number);
	bool GetHexInteger(unsigned* Integer);
	bool GetInteger(int* Integer);
	bool GetBoolean(bool* Boolean);

	// If Key starts with `Start', ignore the section, else ignore the line
	bool Ignore(char* Key);

	// Ignores until newline or byte after a semicolon
	bool IgnoreCharSection();
};


#endif

