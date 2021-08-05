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

#include "pdfDictionary.h"


pdfDictionary::pdfDictionary() {
	First = Last = 0;
	Count = 0;
	OneLine = false;
	Indent = 0;
}


pdfDictionary::~pdfDictionary() {
	Clear();
}


void pdfDictionary::Clear() {
	while (First) {
		Last = First;
		First = First->Next;
		delete Last;
	}
	Last = 0;
	Count = 0;
}


void pdfDictionary::AddEntry(const char* Property, pdfObject* Value) {
	// Search for duplicates
	Element* Temp = First;
	while (Temp) {
		if (Temp->Property.Equivalent(Property)) {
			Temp->Value = Value;
			return;
		}
		Temp = Temp->Next;
	}

	// Otherwise, add new entry
	Temp = new Element;

	Temp->Property.Set(Property);
	Temp->Value = Value;
	Temp->Next = 0;

	if (Last) Last->Next = Temp;
	else     First = Temp;
	Last = Temp;

	Count++;
}


int pdfDictionary::GetCount() {
	return Count;
}


int pdfDictionary::GetLength() {
	int      r = 2;
	Element* Temp = First;

	while (Temp) {
		if (OneLine || Temp->Property.Equal("Subtype")) {
			if (r > 2) r++; // Space between entries
		}
		else {
			r += Indent + 2; // Newline and indent
		}
		r += Temp->Property.GetLength();
		r++; // Space
		r += Temp->Value->GetLength();
		Temp = Temp->Next;
	}
	if (!OneLine) r += Indent + 1; // Newline and Indent
	r += 2;

	return r;
}


int pdfDictionary::GetOutput(char* Buffer) {
	int      j;
	int      i = 0;
	Element* Temp = First;

	Buffer[i++] = '<';
	Buffer[i++] = '<';

	while (Temp) {
		if (OneLine || Temp->Property.Equal("Subtype")) {
			if (i > 2) Buffer[i++] = ' ';
		}
		else {
			Buffer[i++] = '\n';
			for (j = 0; j <= Indent; j++) Buffer[i++] = ' ';
		}
		i += Temp->Property.GetOutput(Buffer + i);
		Buffer[i++] = ' ';
		i += Temp->Value->GetOutput(Buffer + i);
		Temp = Temp->Next;
	}

	if (!OneLine) {
		Buffer[i++] = '\n';
		for (j = 0; j < Indent; j++) Buffer[i++] = ' ';
	}
	Buffer[i++] = '>';
	Buffer[i++] = '>';

	return i;
}


