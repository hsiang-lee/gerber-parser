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

#ifndef PDFFONTFILE_H
#define PDFFONTFILE_H


#include "pdfStream.h"
#include "tools/file_wrapper.h"


class pdfFontFile : public pdfStream {
private:
	pdfNumber Length1;
	pdfNumber Length2;
	pdfNumber Length3;

	void Update();

	bool Starts(const char* s1, const char* s2); // does s1 start with s2?
	void CalculateLengths(char* Buffer, int Length);

public:
	pdfFontFile();

	// Load a PFB file
	bool LoadPFB(const char* FileName);
};


#endif


