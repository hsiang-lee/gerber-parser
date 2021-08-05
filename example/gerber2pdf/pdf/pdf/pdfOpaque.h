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

#ifndef PDFGRAPHICSSTATE_H
#define PDFGRAPHICSSTATE_H


#include "pdfIndirect.h"
#include "pdfDictionary.h"


// Remember to add this to the pdf indirect list and the resource of the page
// or form in which it is used
class pdfOpaque : public pdfIndirect {
private:
	pdfDictionary Dictionary;

	pdfNumber CurrentStrokeAlpha;
	pdfNumber CurrentFillAlpha;

public:
	pdfName Name; // Do not use the name directly

	pdfOpaque(const char* Name);

	void Opacity(double Opacity);
};


#endif


