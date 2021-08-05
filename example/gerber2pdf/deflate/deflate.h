//==============================================================================
// Copyright (C) John-Philip Taylor
// jpt13653903@gmail.com
// This file is part of a library that conforms to the ZLib and Deflate
// standards
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

#ifndef JDEFLATE_H
#define JDEFLATE_H

#include <stdio.h>

class Deflate {
private:
	unsigned char* Buffer;
	unsigned       BufferSize;
	unsigned       Length;

	unsigned WindowSize;

	struct SYMBOL {
		unsigned BitCount;
		unsigned Symbol;
	};

	struct HUFFMAN {
		unsigned Symbol;
		HUFFMAN* Zero;
		HUFFMAN* One;

		HUFFMAN();
		~HUFFMAN();
	};

	void AddToTree(
		HUFFMAN* Root,
		unsigned Symbol,
		unsigned Count,
		unsigned Code
	);
	void BuildTree(HUFFMAN* Root, SYMBOL* Symbols, unsigned Count);
	void DefaultTree(HUFFMAN* LiteralTree, HUFFMAN* DistanceTree);
	void DynamicTree(
		HUFFMAN* LiteralTree,
		HUFFMAN* DistanceTree,
		unsigned char* Buffer,
		unsigned* Index,
		unsigned* BitIndex
	);

	unsigned DecodeSymbol(
		HUFFMAN* Tree,
		unsigned char* Buffer,
		unsigned* Index,
		unsigned* BitIndex
	);
	unsigned LiteralToLength(
		unsigned       Literal,
		unsigned char* Buffer,
		unsigned* Index,
		unsigned* BitIndex
	);
	unsigned DecodeDistance(
		unsigned       Distance,
		unsigned char* Buffer,
		unsigned* Index,
		unsigned* BitIndex
	);

	struct CODE {
		unsigned Code;
		unsigned BitCount;
	};

	struct CODE_ELEMENT {
		union {
			unsigned Literal : 16;
			unsigned Length : 16;
		};
		unsigned Distance : 16; // Literal when Distance = 0;
	};

	struct DYNAMIC_NODE {
		unsigned Frequency;
		unsigned BitLength;
		DYNAMIC_NODE* Next;
		DYNAMIC_NODE* Parent;
		DYNAMIC_NODE* Zero;
		DYNAMIC_NODE* One;

		DYNAMIC_NODE();
		~DYNAMIC_NODE();
	};

	DYNAMIC_NODE* PriorityQueue;
	void     AddToQueue(DYNAMIC_NODE* Node);
	unsigned FindLengths(DYNAMIC_NODE* Tree, unsigned Length);
	void     RebuildQueue(DYNAMIC_NODE* Tree);
	void     BuildCodes(CODE* Codes, SYMBOL* Symbols, unsigned Count);
	void     MakeCodeTree(
		CODE* Codes,
		DYNAMIC_NODE** Nodes,
		unsigned       Count,
		unsigned       BitLengthLimit
	);

	void DefaultCodes(CODE* Literals, CODE* Distances);
	void DynamicCodes(
		CODE_ELEMENT* Elements,
		unsigned      ElementCount,
		CODE* LiteralCodes,
		CODE* DistanceCodes,
		unsigned* Index,
		unsigned* BitIndex
	);

	unsigned LengthToLiteral(unsigned Length, CODE* ExtraBits);
	unsigned EncodeDistance(unsigned Distance, CODE* ExtraBits);
	bool     FindLengthDistancePair(
		unsigned char* Buffer, // The input stream when deflating
		unsigned       Index,  // The place to insert next
		unsigned       BufferLength,
		unsigned* Length,
		unsigned* Distance
	);

	void AddBinary(const unsigned char* Buffer, unsigned Length);

	void AddBits(
		unsigned  Bits,
		unsigned  Count,
		unsigned* Index,
		unsigned* BitIndex,
		bool      BitReverse = false
	);

	unsigned GetBits(
		unsigned char* Buffer,
		unsigned       Count,
		unsigned* Index,
		unsigned* BitIndex,
		bool           BitReverse = false
	);

public:
	Deflate();

	unsigned char* Inflate(unsigned char* Buffer, unsigned* Length);
	unsigned char* RunDeflate(
		unsigned char* Buffer,
		unsigned* Length,
		unsigned       WindowSize = 0x800
	);
};

#endif

