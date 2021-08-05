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

#ifndef PDFRECTANGLE_H
#define PDFRECTANGLE_H


#include <math.h>


#include "pdfArray.h"
#include "pdfNumber.h"


class pdfRectangle : public pdfArray {
public: // Read-only
	pdfNumber left_;
	pdfNumber right_;
	pdfNumber top_;
	pdfNumber bottom_;

public: // Functions
	pdfRectangle();

	void SetA4();
	void SetA4Landscape();
	void Set_mm(double left, double bottom, double right, double top);
	void Set(double left, double bottom, double right, double top);

	bool HasArea();
};


#endif


