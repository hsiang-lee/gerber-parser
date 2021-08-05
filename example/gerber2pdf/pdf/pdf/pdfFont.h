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

#ifndef PDFFONT_H
#define PDFFONT_H


#include "pdfIndirect.h"
#include "pdfDictionary.h"
#include "pdfFontMetrics.h"


// The standard 14 fonts
class pdfFont : public pdfIndirect {
public:
	enum FONT {
		Times_Roman,
		Times_Bold,
		Times_Italic,
		Times_BoldItalic,

		Helvetica,
		Helvetica_Bold,
		Helvetica_Oblique,
		Helvetica_BoldOblique,

		Courier,
		Courier_Bold,
		Courier_Oblique,
		Courier_BoldOblique,

		Symbol,
		ZapfDingbats,
	};

private:
	bool WinANSI;

protected:
	pdfDictionary Dictionary;

	pdfName Type;
	pdfName Subtype;
	pdfName BaseFont;
	pdfName Encoding;

	virtual void Update();

public:
	pdfName Name; // Do not use the name directly

	pdfFont(const char* Name);

	// Load an AFM file into the Metrics for kerning to work
	pdfFontMetrics Metrics;

	void SetFont(FONT Font);
	void SetWinANSI(); // Also calls Metrics.SetWinANSI
};


#endif


