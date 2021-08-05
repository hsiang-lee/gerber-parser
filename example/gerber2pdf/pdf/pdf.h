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

#ifndef PDF_H
#define PDF_H

#include "tools/file_wrapper.h"

#include "pdf/pdfArray.h"
#include "pdf/pdfBool.h"
#include "pdf/pdfCatalogue.h"
#include "pdf/pdfDictionary.h"
#include "pdf/pdfFont.h"
#include "pdf/pdfType1Font.h"
#include "pdf/pdfType3Font.h"
#include "pdf/pdfJPEG.h"
#include "pdf/pdfPNG.h"
#include "pdf/pdfName.h"
#include "pdf/pdfNumber.h"
#include "pdf/pdfPage.h"
#include "pdf/pdfPages.h"
#include "pdf/pdfString.h"
#include "pdf/pdfForm.h"
#include "pdf/pdfHyperlink.h"
#include "pdf/pdfOutlineItems.h"

#define PDF_Version "PDF-1.5"

class Pdf {
private:
	struct Element {
		pdfIndirect* Object;
		unsigned     Offset;
		Element* Next;
	};
	Element* First;
	Element* Last;

	pdfIndirect Info;

	FILE_WRAPPER File;

public:
	Pdf();
	~Pdf();

	pdfString Title;
	pdfString Author;
	pdfString Subject;
	pdfString Keywords;
	pdfString Creator;
	pdfString Producer;
	pdfString CreationDate;

	pdfCatalogue Catalogue;

	// Adds an indirect object to the object list
	void AddIndirect(pdfIndirect* Indirect);

	// Use this to add all the components of the Type1 font to the Indirect list
	void AddType1Font(pdfType1Font* Font);

	// Define the font in full before calling this function
	void AddType3Font(pdfType3Font* Font);

	void WritePDF(const std::string& FileName);
};

#endif

