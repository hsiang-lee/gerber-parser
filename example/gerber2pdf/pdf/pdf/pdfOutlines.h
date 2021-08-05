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

#ifndef PDFOUTLINES_H
#define PDFOUTLINES_H


#include "pdfIndirect.h"
#include "pdfDictionary.h"


class pdfOutlineItems;
class pdfOutlines : public pdfIndirect {
protected:
	pdfDictionary Dictionary;

	// Members of Dictionary:
	pdfName          Type;
	pdfOutlines* Parent; // Used by children only
	pdfOutlineItems* Next;   // Used by children only
	pdfOutlineItems* Prev;   // Used by children only
	pdfOutlineItems* First;
	pdfOutlineItems* Last;
	pdfNumber        Count; // Number of all open items in all children

	virtual void Update();

public:
	pdfOutlines();

	// Build the tree bottom-up
	void AddChild(pdfOutlineItems* Child);
};


#endif


