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

#include "pdfArray.h"


pdfArray::pdfArray() {
	First = 0;
	Last = 0;
	Count = 0;
}


pdfArray::~pdfArray() {
	Clear();
}


void pdfArray::Clear() {
	while (First) {
		Last = First;
		First = First->Next;
		delete Last;
	}
	Last = 0;
	Count = 0;
}


void pdfArray::Add(pdfObject* Object) {
	Element* Temp;

	Temp = new Element;
	Temp->Object = Object;
	Temp->Next = 0;

	if (First) {
		Last->Next = Temp;
	}
	else {
		First = Temp;
	}
	Last = Temp;

	Count++;
}


int pdfArray::GetCount() {
	return Count;
}


int pdfArray::GetLength() {
	int r = 2;
	Element* Temp = First;

	while (Temp) {
		if (r > 2) r++; // No space before the first element
		r += Temp->Object->GetLength();
		Temp = Temp->Next;
	}

	return r;
}


int pdfArray::GetOutput(char* Buffer) {
	int      i = 0;
	Element* Temp = First;

	Buffer[i++] = '[';
	while (Temp) {
		if (i > 1) Buffer[i++] = ' ';
		i += Temp->Object->GetOutput(Buffer + i);
		Temp = Temp->Next;
	}
	Buffer[i++] = ']';

	return i;
}


