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

#include "pdfRectangle.h"


pdfRectangle::pdfRectangle() {
	Add(&left_);
	Add(&bottom_);
	Add(&right_);
	Add(&top_);
}


void pdfRectangle::SetA4() {
	Set_mm(0, 0, 210, 297);
}


void pdfRectangle::SetA4Landscape() {
	Set_mm(0, 0, 297, 210);
}


void pdfRectangle::Set_mm(double left, double bottom, double right, double top) {
	Set(
		left / 25.4 * 72.0,
		bottom / 25.4 * 72.0,
		right / 25.4 * 72.0,
		top / 25.4 * 72.0
	);
}


void pdfRectangle::Set(
	double left,
	double bottom,
	double right,
	double top
) {
	pdfRectangle::left_.Value = left;
	pdfRectangle::bottom_.Value = bottom;
	pdfRectangle::right_.Value = right;
	pdfRectangle::top_.Value = top;
}


bool pdfRectangle::HasArea() {
	if (fabs(right_.Value - left_.Value) > 0.0 &&
		fabs(top_.Value - bottom_.Value) > 0.0) {
		return true;
	}
	return false;
}

