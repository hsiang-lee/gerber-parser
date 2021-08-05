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

#ifndef PDFFORM_H
#define PDFFORM_H


#include "pdfContents.h"
#include "pdfRectangle.h"
#include "pdfResources.h"


// Form XObject
class pdfForm : public pdfContents {
protected:
	pdfName       Type;
	pdfName       SubType;
	pdfDictionary Group;

	// Components of Group
	pdfName GroupSubtype;

public:
	pdfForm(const char* Name, bool UseCMYK = false);

	// The "Name" field is deprecated, but still required as a member of this 
	// class for resources loading purposes.
	pdfName      Name;
	pdfRectangle BBox;

	// Call Update after changing Resources for the first time
	pdfResources Resources;
	virtual void Update();
};


#endif


