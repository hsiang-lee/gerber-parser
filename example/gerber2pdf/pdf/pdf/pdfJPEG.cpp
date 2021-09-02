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

#include "pdfJPEG.h"


pdfJPEG::pdfJPEG(const char* Name) : pdfImage(Name) {
	pdfJPEG::Filter.Set("DCTDecode");

	Update();
}


void pdfJPEG::Update() {
	Dictionary.Clear();
	Dictionary.AddEntry("Type", &Type);
	Dictionary.AddEntry("Subtype", &Subtype);
	Dictionary.AddEntry("Name", &Name);
	Dictionary.AddEntry("Width", &Width);
	Dictionary.AddEntry("Height", &Height);
	Dictionary.AddEntry("ColorSpace", &ColourSpace);
	Dictionary.AddEntry("BitsPerComponent", &BitsPerColour);
	Dictionary.AddEntry("Interpolate", &Interpolate);
	Dictionary.AddEntry("Length", &Length);
	Dictionary.AddEntry("Filter", &Filter);
}


void pdfJPEG::GetSizes(const unsigned char* Buffer, unsigned Length) {
	int      SectionLength;
	int      Marker;
	int      Tasks = 0;
	unsigned Index = 0;

	int Units = 0;
	int XDensity = 0;
	int YDensity = 0;

	int Precision = 0;
	int Width = 0;
	int Height = 0;

	while (Index < Length - 4 && Tasks < 3) {
		if (Buffer[Index++] != 0xFF) return;
		Marker = Buffer[Index++];
		SectionLength = (Buffer[Index] << 8) +
			Buffer[Index + 1];
		switch (Marker) {
		case 0xD8: // Start of image
			Tasks++;
			break;

		case 0xE0: // Application Segment
			if (Buffer[Index + 2] == 'J' &&
				Buffer[Index + 3] == 'F' &&
				Buffer[Index + 4] == 'I' &&
				Buffer[Index + 5] == 'F' &&
				Buffer[Index + 6] == 0) {
				Units = Buffer[Index + 9];
				XDensity = (Buffer[Index + 10] << 8) |
					Buffer[Index + 11];
				YDensity = (Buffer[Index + 12] << 8) |
					Buffer[Index + 13];
				Tasks++;
			}
			Index += SectionLength;
			break;

		case 0xC0:
		case 0xC1:
		case 0xC2:
		case 0xC3:
		case 0xC9:
		case 0xCA:
		case 0xCB:
			Precision = Buffer[Index + 2];
			Height = (Buffer[Index + 3] << 8) |
				Buffer[Index + 4];
			Width = (Buffer[Index + 5] << 8) |
				Buffer[Index + 6];
			Index += SectionLength;
			Tasks++;
			break;

		default:
			Index += SectionLength;
			break;
		}
	}

	switch (Units) {
	case 2: // cm
		Width_mm = (double)Width / (double)XDensity * 10.0;
		Height_mm = (double)Height / (double)YDensity * 10.0;
		break;
	case 0: // None
	case 1: // Inches
	default:
		Width_mm = (double)Width / (double)XDensity * 25.4;
		Height_mm = (double)Height / (double)YDensity * 25.4;
		break;
	}

	BitsPerColour = Precision;
	pdfJPEG::Width = Width;
	pdfJPEG::Height = Height;
}


bool pdfJPEG::LoadFromFile(const char* Filename) {
	int   Length;
	char* Buffer;
	FILE_WRAPPER File;

	if (File.Open(Filename, FILE_WRAPPER::faRead)) {
		Length = File.GetSize();
		Buffer = new char[Length];
		File.Read(Buffer, Length);
		AddBinary((unsigned char*)Buffer, Length);
		GetSizes((unsigned char*)Buffer, Length);
		File.Close();
		delete[] Buffer;
		return true;

	}

	return false;
}

