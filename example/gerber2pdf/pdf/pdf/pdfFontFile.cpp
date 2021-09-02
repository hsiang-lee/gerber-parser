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

#include "pdfFontFile.h"


pdfFontFile::pdfFontFile() {
	Update();
}


void pdfFontFile::Update() {
	Dictionary.Clear();
	Dictionary.AddEntry("Length", &Length);

	if (!Filter.Empty()) Dictionary.AddEntry("Filter", &Filter);

	Dictionary.AddEntry("Length1", &Length1);
	Dictionary.AddEntry("Length2", &Length2);
	Dictionary.AddEntry("Length3", &Length3);
}


bool pdfFontFile::Starts(const char* s1, const char* s2) {
	int j;
	for (j = 0; s2[j]; j++) {
		if (s1[j] != s2[j]) return false;
	}
	return true;
}


void pdfFontFile::CalculateLengths(char* Buffer, int Length) {
	// Length1: length in bytes of the clear-text portion
	// Length2: length in bytes of the encrypted portion
	// Length3: length in bytes of the fixed-content portion

	int j = 0;
	while (j < Length - 17) {
		if (Starts(Buffer + j, "currentfile eexec")) break;
		j++;
	}
	j += 17;
	if (Buffer[j] == '\r') j++;
	if (Buffer[j] == '\n') j++;

	Length1 = j;

	while (j < Length - 10) {
		if (Starts(Buffer + j, "0000000000")) break;
		j++;
	}

	Length2 = j - Length1.Value;
	Length3 = Length - j;
}


bool pdfFontFile::LoadPFB(const char* FileName) {
	int   Length;
	char* Buffer;
	FILE_WRAPPER File;

	if (File.Open(FileName, FILE_WRAPPER::faRead)) {
		Length = File.GetSize();
		Buffer = new char[Length];
		File.Read(Buffer, Length);
		AddBinary((unsigned char*)Buffer, Length);
		File.Close();
		CalculateLengths(Buffer, Length);
		delete[] Buffer;
		return true;

	}

	return false;
}

