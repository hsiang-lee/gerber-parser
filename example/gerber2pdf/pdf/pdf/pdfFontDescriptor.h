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

#ifndef PDFFONTDESCRIPTOR_H
#define PDFFONTDESCRIPTOR_H


#include "pdfDictionary.h"
#include "pdfIndirect.h"
#include "pdfFontMetrics.h"
#include "pdfRectangle.h"
#include "pdfFontFile.h"


class pdfFontDescriptor : public pdfIndirect {
private:
	pdfDictionary Dictionary;

	pdfName      Type;
	pdfName      FontName;
	pdfNumber    Flags;
	pdfRectangle FontBBox;
	pdfNumber    ItalicAngle;
	pdfNumber    Ascent;
	pdfNumber    Descent;
	pdfNumber    CapHeight; // M Height
	pdfNumber    XHeight;
	pdfNumber    StemV;

public:
	pdfFontDescriptor();

	pdfFontFile FontFile;

	void LoadMetrics(pdfFontMetrics* Metrics);
};


#endif


