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

#include "pdfType1Font.h"

pdfType1Font::pdfType1Font(const char* Name) : pdfFont(Name) {
	Widths.Object = &WidthsArray;

	Dictionary.Clear();
	Dictionary.AddEntry("Type", &Type);
	Dictionary.AddEntry("Subtype", &Subtype);
	Dictionary.AddEntry("Widths", &Widths);
	Dictionary.AddEntry("LastChar", &LastChar);
	Dictionary.AddEntry("BaseFont", &BaseFont);
	Dictionary.AddEntry("FirstChar", &FirstChar);
	Dictionary.AddEntry("FontDescriptor", &FontDescriptor);

	TheWidths = 0;
}

pdfType1Font::~pdfType1Font() {
	if (TheWidths) delete[] TheWidths;
}

void pdfType1Font::SetWidths() {
	int j;
	int FirstChar = 0;
	int LastChar = 0;

	FirstChar = 0;
	while (FirstChar < 0x100 && !Metrics.HasGlyph(FirstChar)) {
		FirstChar++;
	}

	LastChar = 0xFF;
	while (LastChar >= 0 && !Metrics.HasGlyph(LastChar)) {
		LastChar--;
	}
	if (LastChar < 0) LastChar = 0;

	if (FirstChar > LastChar) FirstChar = LastChar;
	pdfType1Font::FirstChar = FirstChar;
	pdfType1Font::LastChar = LastChar;

	if (TheWidths) delete[] TheWidths;
	TheWidths = new pdfNumber[LastChar - FirstChar + 1];

	for (j = FirstChar; j <= LastChar; j++) {
		TheWidths[j - FirstChar] = Metrics.Advance(j);
		WidthsArray.Add(&(TheWidths[j - FirstChar]));
	}
}

void pdfType1Font::LoadFont(const char* FileName) {
	int   j;
	char* Buffer;

	for (j = 0; FileName[j]; j++);
	Buffer = new char[j + 5];

	for (j = 0; FileName[j]; j++) {
		Buffer[j] = FileName[j];
	}
	Buffer[j++] = '.';
	Buffer[j++] = 'a';
	Buffer[j++] = 'f';
	Buffer[j++] = 'm';
	Buffer[j] = 0;

	if (Metrics.LoadAFM(Buffer)) {
		Buffer[j - 3] = 'p';
		Buffer[j - 2] = 'f';
		Buffer[j - 1] = 'b';
		FontDescriptor.FontFile.LoadPFB(Buffer);
		FontDescriptor.LoadMetrics(&Metrics);
		BaseFont.Set(Metrics.FontName);
		SetWidths();
	}

	FontDescriptor.FontFile.Deflate();

	delete[] Buffer;
}
