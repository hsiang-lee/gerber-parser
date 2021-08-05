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

#ifndef PDFARRAY_H
#define PDFARRAY_H


#include "pdfObject.h"


class pdfArray : public pdfObject {
private:
	struct Element {
		pdfObject* Object;
		Element* Next;
	};
	Element* First;
	Element* Last;

	int Count;

public:
	pdfArray();
	~pdfArray();

	void Clear();
	void Add(pdfObject* Object);
	int  GetCount();

	int GetLength();
	int GetOutput(char* Buffer);
};


#endif


