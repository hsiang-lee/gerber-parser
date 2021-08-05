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

#include "pdfStream.h"


pdfStream::pdfStream() {
	Object = &Dictionary;
	Length = 0;
	Buffer = new unsigned char[1];
	BufferSize = 1;
	Update();
}


void pdfStream::Update() {
	Dictionary.Clear();
	Dictionary.AddEntry("Length", &Length);

	if (!Filter.Empty())  Dictionary.AddEntry("Filter", &Filter);
}


void pdfStream::Deflate(unsigned WindowSize) {
	unsigned char* Temp = Buffer;
	unsigned       L = Length.Value;

	if (!Filter.Empty()) return;

	Buffer = ZLib.RunDeflate(Temp, &L, WindowSize);

	delete[] Temp;
	BufferSize = L;
	Length = L;

	Filter.Set("FlateDecode");
	Update();
}


void pdfStream::Inflate() {
	unsigned char* Temp = Buffer;
	unsigned       L = Length.Value;

	if (Filter.Empty()) return;

	Buffer = ZLib.Inflate(Temp, &L);
	delete[] Temp;
	BufferSize = L;
	Length = L;

	Filter.Set("");
	Update();
}


void pdfStream::AddBinary(const unsigned char* Buffer, unsigned Length) {
	unsigned       j, l;
	unsigned char* Temp;

	l = pdfStream::Length.Value;

	if (l + Length > BufferSize) {
		while (l + Length > BufferSize) BufferSize <<= 1;
		Temp = new unsigned char[BufferSize];
		for (j = 0; j < l; j++) {
			Temp[j] = pdfStream::Buffer[j];
		}
		delete[] pdfStream::Buffer;
		pdfStream::Buffer = Temp;
	}

	for (j = 0; j < Length; j++) {
		pdfStream::Buffer[l + j] = Buffer[j];
	}

	pdfStream::Length.Value += Length;
}


void pdfStream::AddLine(const char* Line) {
	int j, l;
	unsigned char* Temp;

	for (l = 0; Line[l]; l++);
	l += 1; // Linefeed

	if (Length.Value + l > BufferSize) {
		while (Length.Value + l > BufferSize) BufferSize <<= 1;
		Temp = new unsigned char[BufferSize];
		for (j = 0; j < Length.Value; j++) {
			Temp[j] = Buffer[j];
		}
		delete[] Buffer;
		Buffer = Temp;
	}

	l -= 1;
	for (j = 0; j < l; j++) {
		Buffer[(int)Length.Value + j] = Line[j];
	}
	Buffer[(int)Length.Value + j] = '\n';

	Length.Value += l + 1;
}


int pdfStream::GetBodyLength() {
	int r = 0;

	pdfNumber n;
	n = Reference;
	r += n.GetLength();

	r += 7; // " 0 obj\n"
	r += Dictionary.GetLength();
	r += 8; // "\nstream\n"
	r += Length.Value;
	if (Buffer[(int)Length.Value - 1] != '\n') r += 1;
	r += 9; // "endstream"
	r += 8; // "\nendobj\n"

	return r;
}


int pdfStream::GetBody(char* Buffer) {
	int j;
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

	i += Dictionary.GetOutput(Buffer + i);

	Buffer[i++] = '\n';
	Buffer[i++] = 's';
	Buffer[i++] = 't';
	Buffer[i++] = 'r';
	Buffer[i++] = 'e';
	Buffer[i++] = 'a';
	Buffer[i++] = 'm';
	Buffer[i++] = '\n';

	for (j = 0; j < Length.Value; j++) {
		Buffer[i++] = pdfStream::Buffer[j];
	}
	if (pdfStream::Buffer[j - 1] != '\n') {
		Buffer[i++] = '\n';
	}

	Buffer[i++] = 'e';
	Buffer[i++] = 'n';
	Buffer[i++] = 'd';
	Buffer[i++] = 's';
	Buffer[i++] = 't';
	Buffer[i++] = 'r';
	Buffer[i++] = 'e';
	Buffer[i++] = 'a';
	Buffer[i++] = 'm';

	Buffer[i++] = '\n';
	Buffer[i++] = 'e';
	Buffer[i++] = 'n';
	Buffer[i++] = 'd';
	Buffer[i++] = 'o';
	Buffer[i++] = 'b';
	Buffer[i++] = 'j';
	Buffer[i++] = '\n';

	return i;
}


int pdfStream::GetSize() {
	return Length.Value;
}

