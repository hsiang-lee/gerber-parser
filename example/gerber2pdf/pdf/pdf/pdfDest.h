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

#ifndef PDFDEST_H
#define PDFDEST_H


#include "pdfPage.h"
#include "pdfName.h"
#include "pdfArray.h"
#include "pdfNumber.h"


class pdfDest : public pdfArray {
private:
	pdfPage* Page;
	pdfName   Style; // Display Style
	pdfNumber Number[4];

public:
	void Fit(pdfPage* Page);
	void FitB(pdfPage* Page);
	void FitH(pdfPage* Page, double top);
	void FitV(pdfPage* Page, double left);
	void FitBH(pdfPage* Page, double top);
	void FitBV(pdfPage* Page, double left);
	void XYZ(pdfPage* Page, double left, double top, double zoom);
	void FitR(pdfPage* Page, double left, double bottom, double right, double top);
};


#endif


