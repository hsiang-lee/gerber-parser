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

#include "pdfBool.h"


int pdfBool::GetLength() {
	if (Value) return 4;
	return 5;
}


int pdfBool::GetOutput(char* Buffer) {
	if (Value) {
		Buffer[0] = 't';
		Buffer[1] = 'r';
		Buffer[2] = 'u';
		Buffer[3] = 'e';
		return 4;
	}
	else {
		Buffer[0] = 'f';
		Buffer[1] = 'a';
		Buffer[2] = 'l';
		Buffer[3] = 's';
		Buffer[4] = 'e';
		return 5;
	}
}


