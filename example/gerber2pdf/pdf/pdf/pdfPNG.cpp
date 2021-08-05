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

#include "pdfPNG.h"


pdfPNG::pdfPNG(const char* Name) : pdfImage(Name) {
	pdfPNG::Filter.Set("FlateDecode");

	Predictor = 10;
	Colours = 3;

	DecodeParms.Indent = 1;

	InterpolateImage = false;

	Update();
}


void pdfPNG::Update() {
	Dictionary.Clear();
	Dictionary.AddEntry("Type", &Type);
	Dictionary.AddEntry("Subtype", &Subtype);
	Dictionary.AddEntry("Name", &Name);
	Dictionary.AddEntry("Width", &Width);
	Dictionary.AddEntry("Height", &Height);
	Dictionary.AddEntry("ColorSpace", &ColourSpace);
	Dictionary.AddEntry("BitsPerComponent", &BitsPerColour);

	if (InterpolateImage) Dictionary.AddEntry("Interpolate", &Interpolate);

	Dictionary.AddEntry("Length", &Length);
	Dictionary.AddEntry("Filter", &Filter);
	Dictionary.AddEntry("DecodeParms", &DecodeParms);

	DecodeParms.Clear();
	DecodeParms.AddEntry("Colors", &Colours);
	DecodeParms.AddEntry("Columns", &Width);
	DecodeParms.AddEntry("Predictor", &Predictor);
	DecodeParms.AddEntry("BitsPerComponent", &BitsPerColour);
}


void pdfPNG::SetInterpolate(bool Interpolate) {
	InterpolateImage = Interpolate;
	Update();
}


static bool Equal(const char* s1, const char* s2) {
	int j;
	for (j = 0; s1[j] && s2[j]; j++) {
		if (s1[j] != s2[j]) return false;
	}
	if (s1[j] != s2[j]) return false;
	return true;
}


void pdfPNG::RemoveAlpha() {
	int x, y, j, q;
	int w = Width.Value;
	int h = Height.Value;

	j = q = 0;
	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			if (x == 0) Buffer[q++] = Buffer[j++]; // Filter
			Buffer[q++] = Buffer[j++]; // Red
			Buffer[q++] = Buffer[j++]; // Green
			Buffer[q++] = Buffer[j++]; // Blue
			j++; // Alpha
		}
	}

	Length = q;
}


void pdfPNG::Decode(const unsigned char* Buffer, unsigned Length) {
	char     Marker[5] = { 0, 0, 0, 0, 0 };
	unsigned Index = 0;
	unsigned SectionLength;

	int  BitDepth = 0;
	int  Width = 0;
	int  Height = 0;
	int  ColourType = 0;

#define PaletteUsed (ColourType & 0x1)
#define ColourUsed  (ColourType & 0x2)
#define AlphaUsed   (ColourType & 0x4)

	int Units = 0;
	int XDensity = 300;
	int YDensity = 300;

	// Header 0x: 89 50 4E 47 0D 0A 1A 0A
	if (Buffer[Index++] != 137) return;
	if (Buffer[Index++] != 'P') return;
	if (Buffer[Index++] != 'N') return;
	if (Buffer[Index++] != 'G') return;
	if (Buffer[Index++] != 13) return;
	if (Buffer[Index++] != 10) return;
	if (Buffer[Index++] != 26) return;
	if (Buffer[Index++] != 10) return;

	while (Index < Length - 8) {
		SectionLength = (Buffer[Index] << 24) |
			(Buffer[Index + 1] << 16) |
			(Buffer[Index + 2] << 8) |
			Buffer[Index + 3]; Index += 4;
		Marker[0] = Buffer[Index++];
		Marker[1] = Buffer[Index++];
		Marker[2] = Buffer[Index++];
		Marker[3] = Buffer[Index++];

		if (Equal(Marker, "IHDR")) {
			Width = (Buffer[Index] << 24) |
				(Buffer[Index + 1] << 16) |
				(Buffer[Index + 2] << 8) |
				Buffer[Index + 3]; Index += 4;
			Height = (Buffer[Index] << 24) |
				(Buffer[Index + 1] << 16) |
				(Buffer[Index + 2] << 8) |
				Buffer[Index + 3]; Index += 4;
			BitDepth = Buffer[Index++];
			ColourType = Buffer[Index++];
			Index++; // Compression Method
			Index++; // Filter Method
			Index++; // Interlace Method

		}
		else if (Equal(Marker, "pHYs")) {
			XDensity = (Buffer[Index] << 24) |
				(Buffer[Index + 1] << 16) |
				(Buffer[Index + 2] << 8) |
				Buffer[Index + 3]; Index += 4;
			YDensity = (Buffer[Index] << 24) |
				(Buffer[Index + 1] << 16) |
				(Buffer[Index + 2] << 8) |
				Buffer[Index + 3]; Index += 4;
			Units = Buffer[Index++];

		}
		else if (Equal(Marker, "IDAT")) {
			AddBinary(Buffer + Index, SectionLength);
			Index += SectionLength;

		}
		else if (Equal(Marker, "IEND")) {
			break;

		}
		else {
			Index += SectionLength;
		}
		Index += 4; // The CRC
	}

	switch (Units) {
	case 1: // Meters
		Width_mm = (double)Width / (double)XDensity * 1000.0;
		Height_mm = (double)Height / (double)YDensity * 1000.0;
		break;
	case 0: // None
	default:
		Width_mm = (double)Width / (double)XDensity * 25.4;
		Height_mm = (double)Height / (double)YDensity * 25.4;
		break;
	}

	BitsPerColour = BitDepth;
	pdfPNG::Width = Width;
	pdfPNG::Height = Height;

	if (ColourUsed) Colours = 3;
	if (AlphaUsed) {
		Inflate();
		RemoveAlpha();
		Deflate();
	}
}


bool pdfPNG::LoadFromFile(const char* Filename) {
	int   Length;
	char* Buffer;
	FILE_WRAPPER File;

	if (File.Open(Filename, FILE_WRAPPER::faRead)) {
		Length = File.GetSize();
		Buffer = new char[Length];
		File.Read(Buffer, Length);
		Decode((unsigned char*)Buffer, Length);
		File.Close();
		delete[] Buffer;
		return true;

	}
	else {
		error("%s", GetErrorString(GetLastError()));
	}

	return false;
}
