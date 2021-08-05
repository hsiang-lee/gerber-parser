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

#include "pdfOutlineItems.h"

pdfOutlineItems::pdfOutlineItems() {
	ColourArray.Add(&Red);
	ColourArray.Add(&Green);
	ColourArray.Add(&Blue);

	Update();
}

void pdfOutlineItems::Update() {
	Dictionary.Clear();

	if (Title.Empty()) Title.Set("          ");

	if (Red.Value != 0.0 ||
		Green.Value != 0.0 ||
		Blue.Value != 0.0) Dictionary.AddEntry("C", &ColourArray);
	if (FontFlags.Value != 0.0) Dictionary.AddEntry("F", &FontFlags);

	if (Dest.GetCount()) {
		Dictionary.AddEntry("Dest", &Dest);
	}
	if (Prev) {
		Dictionary.AddEntry("Prev", Prev);
	}
	if (Next) {
		Dictionary.AddEntry("Next", Next);
	}
	if (Last) {
		Dictionary.AddEntry("Last", Last);
	}
	if (First) {
		Dictionary.AddEntry("First", First);
	}
	Dictionary.AddEntry("Title", &Title);
	if (Count.Value != 0) {
		Dictionary.AddEntry("Count", &Count);
	}
	if (Parent) {
		Dictionary.AddEntry("Parent", Parent);
	}
}

void pdfOutlineItems::Colour(double Red, double Green, double Blue) {
	pdfOutlineItems::Red = Red;
	pdfOutlineItems::Green = Green;
	pdfOutlineItems::Blue = Blue;

	Update();
}

void pdfOutlineItems::Italic() {
	int f = FontFlags.Value;
	f |= 1;
	FontFlags = f;

	Update();
}

void pdfOutlineItems::Bold() {
	int f = FontFlags.Value;
	f |= 2;
	FontFlags = f;

	Update();
}

void pdfOutlineItems::DestFit(pdfPage* Page) {
	Dest.Fit(Page);
	Update();
}

void pdfOutlineItems::DestFitB(pdfPage* Page) {
	Dest.FitB(Page);
	Update();
}

void pdfOutlineItems::DestFitH(pdfPage* Page, double Top) {
	Dest.FitH(Page, Top);
	Update();
}

void pdfOutlineItems::DestFitV(pdfPage* Page, double Left) {
	Dest.FitV(Page, Left);
	Update();
}

void pdfOutlineItems::DestFitBH(pdfPage* Page, double Top) {
	Dest.FitBH(Page, Top);
	Update();
}

void pdfOutlineItems::DestFitBV(pdfPage* Page, double Left) {
	Dest.FitBV(Page, Left);
	Update();
}

void pdfOutlineItems::DestXYZ(
	pdfPage* Page,
	double   Left,
	double   Top,
	double   Zoom
) {
	Dest.XYZ(Page, Left, Top, Zoom);
	Update();
}

void pdfOutlineItems::DestFitR(
	pdfPage* Page,
	double   Left,
	double   Bottom,
	double   Right,
	double   Top
) {
	Dest.FitR(Page, Left, Bottom, Right, Top);
	Update();
}
