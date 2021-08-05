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

// The s3f file format:
//  File     = Header "StartFontGlyphs" count {GlyphDef} "EndFontGlyphs";
//  Header   = The "afm" file format until the EndFontMetrics tag;
//  GlyphDef = "StartGlyph" name Graphics "EndGlyph";
//  Graphics = Operator {Argument};
//  Operator:
//   CS      Cap Style  1 = Round
//   JS      Join Style 1 = Round
//   LW      Line width 1 argument
//   Rect    Rectangle  x y w h
//   Circ    Circle     x y r
//   Ellipse Ellipse    x y rx ry
//   BL      Begin Line x y
//   L       Line         x y
//   A       Arc          x y a
//   B       Cubic Bezier c1x c1y c2x c2y x y
//   C       Close
//   S       Stroke
//   F       Fill
//   FS      Fill and Stroke


#ifndef PDFTYPE3FONT_H
#define PDFTYPE3FONT_H

#include "pdfFont.h"
#include "pdfArray.h"
#include "pdfContents.h"
#include "pdfResources.h"
#include "pdfFontDescriptor.h"

#include "../afm/afmParser.h"

/// \todo Remember to implement the Composites section!

class pdfType3Font : public pdfFont {
private:
	bool UseCMYK;

	pdfArray  FontBBox;
	pdfNumber Left;
	pdfNumber Bottom;
	pdfNumber Right;
	pdfNumber Top;

	pdfArray  FontMatrix;
	pdfNumber Zero;
	pdfNumber Thousandth;

	pdfDictionary CharProcsDictionary;

	pdfDictionary EncodingDictionary;
	pdfName       EncodingType;
	pdfArray      Differences;
	pdfNumber* DifferencesCode;
	pdfName* DifferencesName;

	pdfNumber  FirstChar;
	pdfNumber  LastChar;

	pdfArray    WidthsArray;
	pdfNumber* TheWidths;

	afmParser Parser;

	int  Compare(const char* s1, const char* s2);

	pdfContents* GetGlyph();
	pdfContents** GetGlyphs(unsigned* GlyphCount);

	pdfContents** GetFontGlyphs(unsigned* GlyphCount); // The root of the parser

public:
	pdfType3Font(const char* Name, bool UseCMYK = false);
	~pdfType3Font();

	// These are handled by JPDF::AddType3Font and should not be used directly
	pdfIndirect Widths;
	pdfIndirect Encoding;
	pdfIndirect CharProcs;
	void        SetWidths();
	void        SetEncoding(); // Assumes SetWidths has been called

	// Call Update after changing Resources for the first time
	pdfResources Resources;
	void Update();

	// Returns a pointer to the contents structure of the new glyph.  The user is
	// responsible for adding it to the pdf indirect list, drawing the glyph and
	// deleting it after use.
	pdfContents* NewGlyph(
		const char* Name,
		double      Left,
		double      Bottom,
		double      Right,
		double      Top,
		double      Advance
	);

	// Loads a T3F file (Type3 Font).  It has the same specification as an Adobe
	// AFM file, exept that it has a StartFontGlyphs...EndFontGlyphs section
	// appended to the end of the file.  It is the user's responsibility to add
	// the glyphs to the pdf Indirect list and delete the memory afterwards. All
	// glyph streams are deflated.
	pdfContents** LoadT3F(const char* FileName, unsigned* GlyphCount);
};

#endif

