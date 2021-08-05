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

#ifndef PDFNUMBER_H
#define PDFNUMBER_H


#include <math.h>
#include <stdio.h>


#include "pdfObject.h"


class pdfNumber : public pdfObject {
private:
	int IntegerPart;

public:
	pdfNumber();

	double operator= (double Number);

	double Value;

	int GetLength();
	int GetOutput(char* Buffer);
};


#endif


