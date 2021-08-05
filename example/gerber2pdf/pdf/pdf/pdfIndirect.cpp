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

#include "pdfIndirect.h"


pdfIndirect::pdfIndirect() {
	Reference = 0;
	Object = 0;
}


int pdfIndirect::GetLength() {
	int r = 0;

	pdfNumber n;
	n = Reference;
	r += n.GetLength();
	r += 4; // The generation number, spaces and "R"

	return r;
}


int pdfIndirect::GetOutput(char* Buffer) {
	int i = 0;

	pdfNumber n;
	n = Reference;
	i += n.GetOutput(Buffer);

	Buffer[i++] = ' ';
	Buffer[i++] = '0';
	Buffer[i++] = ' ';
	Buffer[i++] = 'R';

	return i;
}


int pdfIndirect::GetBodyLength() {
	int r = 0;

	pdfNumber n;
	n = Reference;
	r += n.GetLength();
	r += 7; // The generation number, "obj" and newline and spaces
	if (Object) r += Object->GetLength();
	r += 8; // Newline and the "endobj" keyword

	return r;
}


int pdfIndirect::GetBody(char* Buffer) {
	int i = 0;

	pdfNumber n;
	n = Reference;
	i += n.GetOutput(Buffer);

	Buffer[i++] = ' ';
	Buffer[i++] = '0';
	Buffer[i++] = ' ';
	Buffer[i++] = 'o';
	Buffer[i++] = 'b';
	Buffer[i++] = 'j';
	Buffer[i++] = ' ';

	if (Object) i += Object->GetOutput(Buffer + i);

	Buffer[i++] = ' ';
	Buffer[i++] = 'e';
	Buffer[i++] = 'n';
	Buffer[i++] = 'd';
	Buffer[i++] = 'o';
	Buffer[i++] = 'b';
	Buffer[i++] = 'j';
	Buffer[i++] = '\n';

	return i;
}

