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

#ifndef PDFPAGE_H
#define PDFPAGE_H


#include "pdfRectangle.h"
#include "pdfIndirect.h"
#include "pdfName.h"
#include "pdfNumber.h"
#include "pdfPages.h"
#include "pdfStream.h"
#include "pdfContents.h"
#include "pdfResources.h"
#include "pdfAnnotation.h"


class pdfPages;
class pdfPage : public pdfIndirect {
private:
	pdfDictionary Dictionary;
	pdfName       Type;
	pdfDictionary Group;
	pdfPages* Parent;
	pdfContents* ContentsPointer;
	pdfArray      Annotations;

	// Components of Group
	pdfName GroupSubtype;
	pdfName GroupColourSpace;

public:
	pdfPage(bool UseCMYK = false);

	// Called by the parent upon adding this page as a child.
	// Do not call directly.
	void SetParent(pdfPages* Parent);

	// Call Update after changing Mediabox or Resources for the first time
	pdfRectangle MediaBox;
	pdfResources Resources;
	void Update();

	// Adds an annotation (such as a hyperlink)
	void AddAnnotation(pdfAnnotation* Annotation);

	// Leave this as external so that two or more pages can have the same contents
	// without duplicating the stream
	void Contents(pdfContents* Contents);
};


#endif


