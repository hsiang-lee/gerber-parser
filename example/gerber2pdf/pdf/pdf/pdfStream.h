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

#ifndef PDFSTREAM_H
#define PDFSTREAM_H


#include "pdfIndirect.h"
#include "pdfNumber.h"
#include "pdfDictionary.h"
#include "deflate/deflate.h"


class pdfStream : public pdfIndirect {
private:
	unsigned int BufferSize;
	Deflate     ZLib;

protected:
	unsigned char* Buffer;

	pdfDictionary  Dictionary;
	pdfNumber      Length;
	pdfName        Filter;

	virtual void Update();

public:
	pdfStream();

	// Encode using ZLib format and sets the Filter
	void Deflate(unsigned WindowSize = 0x800);

	// Decode a ZLib Stream and clears the Filter
	void Inflate();

	void AddBinary(const unsigned char* Buffer, unsigned Length);
	void AddLine(const          char* Line);

	int GetBodyLength();
	int GetBody(char* Buffer);

	int GetSize();
};


#endif


