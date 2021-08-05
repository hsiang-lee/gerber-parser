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

#ifndef PDFPAGES_H
#define PDFPAGES_H


#include "pdfNumber.h"
#include "pdfResources.h"
#include "pdfName.h"
#include "pdfPage.h"
#include "pdfRectangle.h"
#include "pdfStream.h"


class pdfPage;
class pdfPages : public pdfIndirect {
private:
	pdfDictionary Dictionary;
	pdfName       Type;
	pdfPages* Parent;
	pdfArray      Children;
	pdfNumber     Count;

public:
	pdfPages();

	// Called by the parent upon adding these pages to the parent:
	void SetParent(pdfPages* Parent);

	// Call Update after changing Mediabox or Resources for the first time
	pdfRectangle MediaBox;
	pdfResources Resources;
	void Update();

	// Build the tree bottom-up
	void AddChild(pdfPages* Child);
	void AddChild(pdfPage* Page);
};


#endif


