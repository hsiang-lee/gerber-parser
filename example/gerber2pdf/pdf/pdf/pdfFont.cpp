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

#include "pdfFont.h"


pdfFont::pdfFont(const char* Name) {
	Object = &Dictionary;

	WinANSI = false;

	Type.Set("Font");
	Subtype.Set("Type1");
	Encoding.Set("WinAnsiEncoding");

	pdfFont::Name.Set(Name);

	SetFont(Times_Roman);
}


void pdfFont::Update() {
	Dictionary.Clear();
	Dictionary.AddEntry("Type", &Type);
	Dictionary.AddEntry("Subtype", &Subtype);
	Dictionary.AddEntry("BaseFont", &BaseFont);

	if (WinANSI) Dictionary.AddEntry("Encoding", &Encoding);
}


void pdfFont::SetFont(FONT Font) {
	switch (Font) {
	case Times_Roman:
		BaseFont.Set("Times-Roman");
		break;
	case Times_Bold:
		BaseFont.Set("Times-Bold");
		break;
	case Times_Italic:
		BaseFont.Set("Times-Italic");
		break;
	case Times_BoldItalic:
		BaseFont.Set("Times-BoldItalic");
		break;

	case Helvetica:
		BaseFont.Set("Helvetica");
		break;
	case Helvetica_Bold:
		BaseFont.Set("Helvetica-Bold");
		break;
	case Helvetica_Oblique:
		BaseFont.Set("Helvetica-Oblique");
		break;
	case Helvetica_BoldOblique:
		BaseFont.Set("Helvetica-BoldOblique");
		break;

	case Courier:
		BaseFont.Set("Courier");
		break;
	case Courier_Bold:
		BaseFont.Set("Courier-Bold");
		break;
	case Courier_Oblique:
		BaseFont.Set("Courier-Oblique");
		break;
	case Courier_BoldOblique:
		BaseFont.Set("Courier-BoldOblique");
		break;

	case Symbol:
		BaseFont.Set("Symbol");
		WinANSI = false;
		break;
	case ZapfDingbats:
		BaseFont.Set("ZapfDingbats");
		WinANSI = false;
		break;
	default:
		BaseFont.Set("Times-Roman");
		break;
	}

	Update();
}


void pdfFont::SetWinANSI() {
	WinANSI = true;
	Metrics.SetWinANSI();
	Update();
}

