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

#ifndef PDFNAME_H
#define PDFNAME_H

#include "pdfObject.h"

class pdfName : public pdfObject {
private:
	unsigned char* Value;

public:
	pdfName();
	~pdfName();

	void  Set(const char* Value); // Sanitises the string before assignment
	char* Get();                  // Returns the sanitised string

	bool Empty();
	bool Equal(const char* String); // Compares directly (no sanitisation)
	bool Equivalent(const char* String); // Compares with sanitisation

	int GetLength();
	int GetOutput(char* Buffer);
};

#endif

