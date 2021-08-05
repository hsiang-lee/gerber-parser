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

#include "pdfCatalogue.h"


pdfCatalogue::pdfCatalogue() {
	Object = &Dictionary;
	Type.Set("Catalog");
	Pages = 0;
	Outlines = 0;

	SetPageLayout(SinglePage);
	SetPageMode(UseOutlines);

	Update();
}


void pdfCatalogue::Update() {
	Dictionary.Clear();

	Dictionary.AddEntry("Type", &Type);
	if (Pages) {
		Dictionary.AddEntry("Pages", Pages);
	}
	if (Outlines) {
		Dictionary.AddEntry("Outlines", Outlines);
	}
	Dictionary.AddEntry("PageMode", &PageMode);
	Dictionary.AddEntry("PageLayout", &PageLayout);
}


void pdfCatalogue::SetPages(pdfPages* Pages) {
	pdfCatalogue::Pages = Pages;
	Update();
}


void pdfCatalogue::SetOutlines(pdfOutlines* Outlines) {
	pdfCatalogue::Outlines = Outlines;
	Update();
}


void pdfCatalogue::SetPageLayout(PAGE_LAYOUT Layout) {
	switch (Layout) {
	case SinglePage:
		PageLayout.Set("SinglePage");
		break;

	case OneColumn:
		PageLayout.Set("OneColumn");
		break;

	case TwoColumnLeft:
		PageLayout.Set("TwoColumnLeft");
		break;

	case TwoColumnRight:
		PageLayout.Set("TwoColumnRight");
		break;

	case TwoPageLeft:
		PageLayout.Set("TwoPageLeft");
		break;

	case TwoPageRight:
		PageLayout.Set("TwoPageRight");
		break;

	default:
		PageLayout.Set("SinglePage");
		break;

	};
}


void pdfCatalogue::SetPageMode(PAGE_MODE Mode) {
	switch (Mode) {
	case UseNone:
		PageMode.Set("UseNone");
		break;

	case UseOutlines:
		PageMode.Set("UseOutlines");
		break;

	case UseThumbs:
		PageMode.Set("UseThumbs");
		break;

	case FullScreen:
		PageMode.Set("FullScreen");
		break;

	default:
		PageMode.Set("UseNone");
		break;
	}
}

