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

// To use a hyperlink, add it to a page using pdfPage::AddAnnotation


#ifndef PDFHYPERLINK_H
#define PDFHYPERLINK_H


#include "pdfPage.h"
#include "pdfDest.h"
#include "pdfString.h"
#include "pdfAnnotation.h"


class pdfHyperlink : public pdfAnnotation {
private:
	pdfDest       Dest;
	pdfDictionary Action;
	pdfArray      Border;

	pdfNumber Zero; // The number Zero, used in Border

	// Members of Action
	pdfName   URISubtype;
	pdfString URI;

	virtual void Update();

public:
	pdfHyperlink();

	pdfNumber BorderWidth;

	// All length units are in mm
	void DestFit(pdfPage* Page);
	void DestFitB(pdfPage* Page);
	void DestFitH(pdfPage* Page, double top);
	void DestFitV(pdfPage* Page, double left);
	void DestFitBH(pdfPage* Page, double top);
	void DestFitBV(pdfPage* Page, double left);
	void DestXYZ(pdfPage* Page, double left, double top, double zoom);
	void DestFitR(pdfPage* Page, double left, double bottom,
		double right, double top);

	void DestURI(const char* URI); // External Web Link
};


#endif


