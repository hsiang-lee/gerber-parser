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

#include "pdfFontDescriptor.h"


pdfFontDescriptor::pdfFontDescriptor() {
	Type.Set("FontDescriptor");
	Flags = 0x4; // Symbolic

	Object = &Dictionary;

	Dictionary.AddEntry("Type", &Type);
	Dictionary.AddEntry("Flags", &Flags);
	Dictionary.AddEntry("StemV", &StemV);
	Dictionary.AddEntry("Ascent", &Ascent);
	Dictionary.AddEntry("Descent", &Descent);
	Dictionary.AddEntry("XHeight", &XHeight);
	Dictionary.AddEntry("FontName", &FontName);
	Dictionary.AddEntry("FontBBox", &FontBBox);
	Dictionary.AddEntry("FontFile", &FontFile);
	Dictionary.AddEntry("CapHeight", &CapHeight);
	Dictionary.AddEntry("ItalicAngle", &ItalicAngle);
}


void pdfFontDescriptor::LoadMetrics(pdfFontMetrics* Metrics) {
	if (!Metrics) return;

	FontName.Set(Metrics->FontName);

	FontBBox.Set(
		Metrics->Box.Left,
		Metrics->Box.Bottom,
		Metrics->Box.Right,
		Metrics->Box.Top
	);

	Ascent = Metrics->Box.Top;
	Descent = Metrics->Box.Bottom;
	CapHeight = Metrics->Height('M');
	XHeight = Metrics->Height('x');
}

