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

#ifndef PDFOUTLINEITEMS_H
#define PDFOUTLINEITEMS_H

#include "pdfOutlines.h"
#include "pdfDest.h"
#include "pdfPage.h"

class pdfOutlines;
class pdfOutlineItems : public pdfOutlines {
private:
	// Members of Dictionary:
	//  pdfOutlines::Next;
	//  pdfOutlines::Prev;
	//  pdfOutlines::First;
	//  pdfOutlines::Last;
	//  pdfOutlines::Parent;
	//  pdfOutlines::Count; -> Number of children, negative it they are closed
	pdfDest          Dest;
	pdfArray         ColourArray;
	pdfNumber        FontFlags;

	// Members of Colour:
	pdfNumber Red;
	pdfNumber Green;
	pdfNumber Blue;

	void Update();

public:
	pdfOutlineItems();

	pdfString Title;

	void Colour(double Red, double Green, double Blue);
	void Bold();
	void Italic();

	// All length units are in mm
	void DestFit(pdfPage* Page);
	void DestFitB(pdfPage* Page);
	void DestFitH(pdfPage* Page, double Top);
	void DestFitV(pdfPage* Page, double Left);
	void DestFitBH(pdfPage* Page, double Top);
	void DestFitBV(pdfPage* Page, double Left);
	void DestXYZ(pdfPage* Page, double Left, double Top, double Zoom);
	void DestFitR(pdfPage* Page, double Left, double Bottom,
		double Right, double Top);
};

#endif

