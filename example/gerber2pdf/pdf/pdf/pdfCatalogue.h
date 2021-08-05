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

#ifndef PDFCATALOGUE_H
#define PDFCATALOGUE_H


#include "pdfIndirect.h"
#include "pdfDictionary.h"
#include "pdfPages.h"
#include "pdfOutlines.h"


class pdfCatalogue : public pdfIndirect {
public:
	enum PAGE_LAYOUT {
		SinglePage,
		OneColumn,
		TwoColumnLeft,
		TwoColumnRight,
		TwoPageLeft,
		TwoPageRight
	};

	enum PAGE_MODE {
		UseNone,
		UseOutlines,
		UseThumbs,
		FullScreen
	};

private:
	pdfDictionary Dictionary;
	pdfName       Type;
	pdfPages* Pages;
	pdfOutlines* Outlines;
	pdfName       PageLayout;
	pdfName       PageMode;

	void Update();

public:

	pdfCatalogue();

	void SetPages(pdfPages* Pages);
	void SetOutlines(pdfOutlines* Outlines);

	void SetPageLayout(PAGE_LAYOUT Layout);
	void SetPageMode(PAGE_MODE   Mode);
};


#endif


