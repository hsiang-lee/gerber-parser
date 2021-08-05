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

#ifndef PDFTYPE1FONT_H
#define PDFTYPE1FONT_H

#include "pdfFont.h"
#include "pdfFontDescriptor.h"

class pdfType1Font : public pdfFont {
private:
	pdfNumber FirstChar;
	pdfNumber LastChar;
	pdfArray  WidthsArray;

	pdfNumber* TheWidths;

	void SetWidths();

public:
	pdfType1Font(const char* Name);
	~pdfType1Font();

	// These are handled by JPDF::AddType1Font and should not be used directly
	pdfIndirect       Widths;
	pdfFontDescriptor FontDescriptor;

	// - Loads a font program into the FontProgram stream, as well as loading the
	//   font metrics into Metrics.  Automatically deflates the font file.
	// - Do not add the file extension to the filename
	void LoadFont(const char* FileName);
};

#endif

