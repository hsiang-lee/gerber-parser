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

#include "deflate.h"

// For what data size (Length-encoded but not Huffman yet) do you want to start
// using a dynamic tree instead of the fixed default?

#define FixedTreeLimit 100

Deflate::Deflate() {
	Length = 0;
	Buffer = 0;
	BufferSize = 0;
	WindowSize = 0x800;
}

Deflate::HUFFMAN::HUFFMAN() {
	Symbol = 0;
	Zero = 0;
	One = 0;
}

Deflate::HUFFMAN::~HUFFMAN() {
	if (Zero) delete Zero;
	if (One) delete One;
}

Deflate::DYNAMIC_NODE::DYNAMIC_NODE() {
	Frequency = 0;
	BitLength = 0;
	Next = 0;
	Parent = 0;
	Zero = 0;
	One = 0;
}

Deflate::DYNAMIC_NODE::~DYNAMIC_NODE() {
	if (Zero) delete Zero;
	if (One) delete One;
}

void Deflate::DefaultCodes(CODE* Literals, CODE* Distances) {
	unsigned j, c;

	j = 0;
	c = 0x30;
	while (j < 144) {
		Literals[j].Code = c;
		Literals[j].BitCount = 8;
		j++;
		c++;
	}
	c = 0x190;
	while (j < 256) {
		Literals[j].Code = c;
		Literals[j].BitCount = 9;
		j++;
		c++;
	}
	c = 0;
	while (j < 280) {
		Literals[j].Code = c;
		Literals[j].BitCount = 7;
		j++;
		c++;
	}
	c = 0xC0;
	while (j < 288) {
		Literals[j].Code = c;
		Literals[j].BitCount = 8;
		j++;
		c++;
	}

	for (j = 0; j < 32; j++) {
		Distances[j].Code = j;
		Distances[j].BitCount = 5;
	}
}

void Deflate::AddBinary(const unsigned char* Buffer, unsigned Length) {
	unsigned       j, l;
	unsigned char* Temp;

	l = Deflate::Length;

	if (l + Length > BufferSize) {
		while (l + Length > BufferSize) BufferSize <<= 1;
		Temp = new unsigned char[BufferSize];
		for (j = 0; j < l; j++) {
			Temp[j] = Deflate::Buffer[j];
		}
		delete[] Deflate::Buffer;
		Deflate::Buffer = Temp;
	}

	for (j = 0; j < Length; j++) {
		Deflate::Buffer[l + j] = Buffer[j];
	}

	Deflate::Length += Length;
}

void Deflate::AddBits(
	unsigned  Bits,
	unsigned  Count,
	unsigned* Index, // Indices assumed to be at the end of the buffer
	unsigned* BitIndex,
	bool      BitReverse
) {
	unsigned       j, l, nl, t;
	unsigned char* Temp;

	if (Count == 0) return;

	l = Length;
	nl = *Index + 1;
	j = *BitIndex + Count;
	while (j > 8) {
		nl += 1;
		j -= 8;
	}

	if (nl > BufferSize) {
		while (nl > BufferSize) BufferSize <<= 1;
		Temp = new unsigned char[BufferSize];
		for (j = 0; j < l; j++) {
			Temp[j] = Buffer[j];
		}
		for (; j < BufferSize; j++) {
			Temp[j] = 0;
		}
		delete[] Buffer;
		Buffer = Temp;
	}

	if (BitReverse) {
		j = Count;
		t = Bits;
		Bits = 0;
		while (j) {
			Bits <<= 1;
			Bits |= t & 1;
			t >>= 1;
			j--;
		}
	}

	j = 0;
	Bits <<= *BitIndex;
	while (Bits) {
		Buffer[(*Index) + j] |= Bits & 0xFF;
		Bits >>= 8;
		j++;
	}

	*BitIndex += Count;
	while (*BitIndex >= 8) {
		*Index += 1;
		*BitIndex -= 8;
	}

	Length = nl;
}

bool Deflate::FindLengthDistancePair(
	unsigned char* Buffer, // The input stream when deflating
	unsigned       Index,  // The place to insert next
	unsigned       BufferLength,
	unsigned* Length,
	unsigned* Distance
) {
	unsigned length;
	unsigned distance;
	unsigned Buf_Index = BufferLength - Index;
	unsigned Index_Distance;

	*Length = 0;
	*Distance = 0;

	for (distance = 1; distance <= Index && distance <= WindowSize; distance++) {
		Index_Distance = Index - distance;
		for (length = 0; length < Buf_Index && length < 258; length++) {
			if (Buffer[Index + length] != Buffer[Index_Distance + length]) break;
		}
		if (*Length < length) {
			*Length = length;
			*Distance = distance;
		}
		if (length == Buf_Index || length == 258) break;
	}

	return *Length >= 3;
}

unsigned Deflate::LengthToLiteral(unsigned Length, CODE* ExtraBits) {
	ExtraBits->Code = 0;
	ExtraBits->BitCount = 0;

	if (Length < 11) {
		Length += 254;

	}
	else if (Length < 19) {
		Length -= 11;
		ExtraBits->BitCount = 1;
		ExtraBits->Code = Length & 0x1;
		Length >>= 1;
		Length += 265;

	}
	else if (Length < 35) {
		Length -= 19;
		ExtraBits->BitCount = 2;
		ExtraBits->Code = Length & 0x3;
		Length >>= 2;
		Length += 269;

	}
	else if (Length < 67) {
		Length -= 35;
		ExtraBits->BitCount = 3;
		ExtraBits->Code = Length & 0x7;
		Length >>= 3;
		Length += 273;

	}
	else if (Length < 131) {
		Length -= 67;
		ExtraBits->BitCount = 4;
		ExtraBits->Code = Length & 0xF;
		Length >>= 4;
		Length += 277;

	}
	else if (Length < 258) {
		Length -= 131;
		ExtraBits->BitCount = 5;
		ExtraBits->Code = Length & 0x1F;
		Length >>= 5;
		Length += 281;

	}
	else {
		Length = 285;
	}

	return Length;
}

unsigned Deflate::EncodeDistance(unsigned Distance, CODE* ExtraBits) {
	ExtraBits->Code = 0;
	ExtraBits->BitCount = 0;

	if (Distance < 5) {
		Distance -= 1;

	}
	else if (Distance < 9) {
		Distance -= 5;
		ExtraBits->BitCount = 1;
		ExtraBits->Code = Distance & 0x1;
		Distance >>= 1;
		Distance += 4;

	}
	else if (Distance < 17) {
		Distance -= 9;
		ExtraBits->BitCount = 2;
		ExtraBits->Code = Distance & 0x3;
		Distance >>= 2;
		Distance += 6;

	}
	else if (Distance < 33) {
		Distance -= 17;
		ExtraBits->BitCount = 3;
		ExtraBits->Code = Distance & 0x7;
		Distance >>= 3;
		Distance += 8;

	}
	else if (Distance < 65) {
		Distance -= 33;
		ExtraBits->BitCount = 4;
		ExtraBits->Code = Distance & 0xF;
		Distance >>= 4;
		Distance += 10;

	}
	else if (Distance < 129) {
		Distance -= 65;
		ExtraBits->BitCount = 5;
		ExtraBits->Code = Distance & 0x1F;
		Distance >>= 5;
		Distance += 12;

	}
	else if (Distance < 257) {
		Distance -= 129;
		ExtraBits->BitCount = 6;
		ExtraBits->Code = Distance & 0x3F;
		Distance >>= 6;
		Distance += 14;

	}
	else if (Distance < 513) {
		Distance -= 257;
		ExtraBits->BitCount = 7;
		ExtraBits->Code = Distance & 0x7F;
		Distance >>= 7;
		Distance += 16;

	}
	else if (Distance < 1025) {
		Distance -= 513;
		ExtraBits->BitCount = 8;
		ExtraBits->Code = Distance & 0xFF;
		Distance >>= 8;
		Distance += 18;

	}
	else if (Distance < 2049) {
		Distance -= 1025;
		ExtraBits->BitCount = 9;
		ExtraBits->Code = Distance & 0x1FF;
		Distance >>= 9;
		Distance += 20;

	}
	else if (Distance < 4097) {
		Distance -= 2049;
		ExtraBits->BitCount = 10;
		ExtraBits->Code = Distance & 0x3FF;
		Distance >>= 10;
		Distance += 22;

	}
	else if (Distance < 8193) {
		Distance -= 4097;
		ExtraBits->BitCount = 11;
		ExtraBits->Code = Distance & 0x7FF;
		Distance >>= 11;
		Distance += 24;

	}
	else if (Distance < 16385) {
		Distance -= 8193;
		ExtraBits->BitCount = 12;
		ExtraBits->Code = Distance & 0xFFF;
		Distance >>= 12;
		Distance += 26;

	}
	else if (Distance < 32769) {
		Distance -= 16385;
		ExtraBits->BitCount = 13;
		ExtraBits->Code = Distance & 0x1FFF;
		Distance >>= 13;
		Distance += 28;
	}

	return Distance;
}

void Deflate::AddToQueue(DYNAMIC_NODE* Node) {
	DYNAMIC_NODE* Next;
	DYNAMIC_NODE* Prev = 0;

	if (PriorityQueue) {
		Next = PriorityQueue;
		while (Next) {
			if (Next->Frequency > Node->Frequency) break;
			Prev = Next;
			Next = Next->Next;
		}
		Node->Next = Next;
		if (Prev) {
			Prev->Next = Node;
		}
		else {
			PriorityQueue = Node;
		}

	}
	else {
		PriorityQueue = Node;
		Node->Next = 0;
	}
}

unsigned Deflate::FindLengths(DYNAMIC_NODE* Tree, unsigned Length) {
	unsigned l1, l2;

	Tree->BitLength = Length;

	if (Tree->Zero) {
		l1 = FindLengths(Tree->Zero, Length + 1);
	}
	else {
		l1 = Length;
	}

	if (Tree->One) {
		l2 = FindLengths(Tree->One, Length + 1);
	}
	else {
		l2 = Length;
	}

	if (l1 > l2) return l1;
	return l2;
}

void Deflate::RebuildQueue(DYNAMIC_NODE* Tree) {
	if (Tree->Zero && Tree->One) {
		RebuildQueue(Tree->Zero);
		RebuildQueue(Tree->One);
		Tree->Zero = 0;
		Tree->One = 0;
		delete Tree;

	}
	else {
		AddToQueue(Tree);
	}
}

void Deflate::BuildCodes(CODE* Codes, SYMBOL* Symbols, unsigned Count) {
	SYMBOL    Temp;
	unsigned  j, q, w;
	unsigned  MaxBitLength;
	unsigned* BitLengthCount;
	unsigned* StartCode;

	// Sort
	for (j = 0; j < Count; j++) {
		for (q = j + 1; q < Count; q++) {
			if (Symbols[j].BitCount > Symbols[q].BitCount) {
				Temp = Symbols[j];
				Symbols[j] = Symbols[q];
				Symbols[q] = Temp;
			}
			else if (Symbols[j].BitCount == Symbols[q].BitCount) {
				if (Symbols[j].Symbol > Symbols[q].Symbol) {
					Temp = Symbols[j];
					Symbols[j] = Symbols[q];
					Symbols[q] = Temp;
				}
			}
		}
	}

	// Count the number of codes for each code length
	MaxBitLength = Symbols[Count - 1].BitCount;
	BitLengthCount = new unsigned[MaxBitLength + 1];
	for (j = 0; j < MaxBitLength; j++) {
		BitLengthCount[j] = 0;
	}
	for (j = 0; j < Count; j++) {
		BitLengthCount[Symbols[j].BitCount]++;
	}
	BitLengthCount[0] = 0;

	// Find the numerical value of the start code for each set of bit lengths
	q = 0;
	StartCode = new unsigned[MaxBitLength + 1];
	StartCode[0] = 0;
	for (j = 1; j <= MaxBitLength; j++) {
		q = (q + BitLengthCount[j - 1]) << 1;
		StartCode[j] = q;
	}

	// Assign the codes to the symbols
	for (j = 0; Symbols[j].BitCount == 0; j++) {
		Codes[Symbols[j].Symbol].Code = 0;
		Codes[Symbols[j].Symbol].BitCount = 0;
	}
	w = 0;
	q = 0;
	for (; j < Count; j++) {
		if (Symbols[j].BitCount > w) {
			w = Symbols[j].BitCount;
			q = StartCode[w];
		}
		Codes[Symbols[j].Symbol].Code = q;
		Codes[Symbols[j].Symbol].BitCount = Symbols[j].BitCount;
		q++;
	}

	delete[] BitLengthCount;
	delete[] StartCode;
}

void Deflate::MakeCodeTree(
	CODE* Codes,
	DYNAMIC_NODE** Nodes,
	unsigned       Count,
	unsigned       BitLengthLimit
) {
	unsigned j;
	unsigned MaxLength;

	DYNAMIC_NODE* Tree;
	DYNAMIC_NODE* Temp;

	SYMBOL* Symbols = new SYMBOL[Count];

	// Build the queue
	PriorityQueue = 0;
	for (j = 0; j < Count; j++) {
		if (Nodes[j]->Frequency) {
			AddToQueue(Nodes[j]);
		}
		else {
			delete Nodes[j];
			Nodes[j] = 0;
		}
	}

	// Build the tree by removing and maintaining the queue
	while (PriorityQueue && PriorityQueue->Next) {
		Temp = new DYNAMIC_NODE;
		Temp->Parent = 0;
		Temp->Zero = PriorityQueue;
		Temp->Zero->Parent = Temp;
		PriorityQueue = PriorityQueue->Next;
		Temp->One = PriorityQueue;
		Temp->One->Parent = Temp;
		PriorityQueue = PriorityQueue->Next;
		Temp->Frequency = Temp->Zero->Frequency + Temp->One->Frequency;
		AddToQueue(Temp);
	}
	Tree = PriorityQueue;
	PriorityQueue = 0;

	// Walk the tree to find the bitlengths of the leaves
	MaxLength = FindLengths(Tree, 0);

	// Modify the tree such that the maximum bitlength is 15
	while (MaxLength > BitLengthLimit) {
		// Attempt to move the bottom-most leaves up a level
		for (j = 0; j < Count; j++) {
			if (Nodes[j]) {
				if (Nodes[j]->BitLength == MaxLength) {
					Temp = Nodes[j];
					Temp->Frequency = Temp->Parent->Frequency;
				}
			}
		}

		// Delete the nodes and add the leaves to the queue
		RebuildQueue(Tree);
		// Build the tree
		while (PriorityQueue && PriorityQueue->Next) {
			Temp = new DYNAMIC_NODE;
			Temp->Parent = 0;
			Temp->Zero = PriorityQueue;
			Temp->Zero->Parent = Temp;
			PriorityQueue = PriorityQueue->Next;
			Temp->One = PriorityQueue;
			Temp->One->Parent = Temp;
			PriorityQueue = PriorityQueue->Next;
			Temp->Frequency = Temp->Zero->Frequency + Temp->One->Frequency;
			AddToQueue(Temp);
		}
		Tree = PriorityQueue;
		PriorityQueue = 0;

		MaxLength = FindLengths(Tree, 0);
	}

	// Build a SYMBOL Array
	for (j = 0; j < Count; j++) {
		if (Nodes[j]) {
			Symbols[j].Symbol = j;
			Symbols[j].BitCount = Nodes[j]->BitLength;
		}
		else {
			Symbols[j].Symbol = j;
			Symbols[j].BitCount = 0;
		}
	}

	// Generate the Codes from the SYMBOL array
	BuildCodes(Codes, Symbols, Count);

	// No need to add the whole list
	delete   Tree;
	delete[] Symbols;
}

static unsigned CompressCodeLengths(unsigned* Codes, unsigned Length) {
	unsigned Source = 0;
	unsigned Dest = 0;
	unsigned RepeatCount;
	unsigned TempCount;
	unsigned TempCode;

	while (Source < Length) {
		TempCode = Codes[Source];
		if (TempCode) {
			if (Source) {
				RepeatCount = 0;
				while (Source + RepeatCount < Length) {
					if (Codes[Source + RepeatCount] != Codes[Source - 1]) break;
					RepeatCount++;
				}
			}
			if (!RepeatCount) RepeatCount++; // No repeat, so put one literal
			while (RepeatCount >= 3) {
				Codes[Dest++] = 16; // Code for repeat
				TempCount = (RepeatCount - 3) & 0x3;
				Codes[Dest++] = TempCount;
				TempCount += 3;
				RepeatCount -= TempCount;
				Source += TempCount;
			}
			while (RepeatCount) {
				Codes[Dest++] = TempCode;
				RepeatCount--;
				Source++;
			}

		}
		else { // Null
			RepeatCount = 0;
			while (Source + RepeatCount < Length) {
				if (Codes[Source + RepeatCount]) break;
				RepeatCount++;
			}
			if (!RepeatCount) RepeatCount++; // No repeat, so put one literal
			while (RepeatCount >= 11) {
				Codes[Dest++] = 18; // Code for repeat null
				TempCount = (RepeatCount - 11) & 0x7F;
				Codes[Dest++] = TempCount;
				TempCount += 11;
				RepeatCount -= TempCount;
				Source += TempCount;
			}
			while (RepeatCount >= 3) {
				Codes[Dest++] = 17; // Code for repeat null
				TempCount = (RepeatCount - 3) & 0x7;
				Codes[Dest++] = TempCount;
				TempCount += 3;
				RepeatCount -= TempCount;
				Source += TempCount;
			}
			while (RepeatCount) {
				Codes[Dest++] = TempCode;
				RepeatCount--;
				Source++;
			}
		}
	}

	return Dest;
}

void Deflate::DynamicCodes(
	CODE_ELEMENT* Elements,
	unsigned      ElementCount,
	CODE* LiteralCodes,
	CODE* DistanceCodes,
	unsigned* Index,
	unsigned* BitIndex
) {
	unsigned j, q;
	unsigned NLiterals;
	unsigned NDistances;
	unsigned NCodeLengths;
	unsigned NCodeLengthCodes;

	unsigned* CodeLengths;

	DYNAMIC_NODE* Frequency[286];

	CODE ExtraBits;
	CODE CodeLengthCodes[19];

	NLiterals = 286;
	NDistances = 30;
	NCodeLengthCodes = 19;

	// Get the Literal/Length Codes
	for (j = 0; j < NLiterals; j++) {
		Frequency[j] = new DYNAMIC_NODE;
	}
	for (j = 0; j < ElementCount; j++) {
		if (Elements[j].Distance) {
			q = LengthToLiteral(Elements[j].Length, &ExtraBits);
		}
		else {
			q = Elements[j].Literal;
		}
		Frequency[q]->Frequency++;
	}
	Frequency[0x100]->Frequency++; // End of block

	MakeCodeTree(LiteralCodes, Frequency, NLiterals, 15);
	while (NLiterals > 257 && LiteralCodes[NLiterals - 1].BitCount == 0) NLiterals--;

	// Get the Distance Codes
	for (j = 0; j < NDistances; j++) {
		Frequency[j] = new DYNAMIC_NODE;
	}
	for (j = 0; j < ElementCount; j++) {
		if (Elements[j].Distance) {
			q = EncodeDistance(Elements[j].Distance, &ExtraBits);
			Frequency[q]->Frequency++;
		}
	}

	MakeCodeTree(DistanceCodes, Frequency, NDistances, 15);
	while (NDistances > 1 && DistanceCodes[NDistances - 1].BitCount == 0)
		NDistances--;

	// Calculate the Code Length Codes
	NCodeLengths = NLiterals + NDistances;
	CodeLengths = new unsigned[NCodeLengths];
	q = 0;
	for (j = 0; j < NLiterals; j++) {
		CodeLengths[q++] = LiteralCodes[j].BitCount;
	}
	for (j = 0; j < NDistances; j++) {
		CodeLengths[q++] = DistanceCodes[j].BitCount;
	}
	NCodeLengths = CompressCodeLengths(CodeLengths, NCodeLengths);

	for (j = 0; j < NCodeLengthCodes; j++) {
		Frequency[j] = new DYNAMIC_NODE;
	}
	for (j = 0; j < NCodeLengths; j++) {
		Frequency[CodeLengths[j]]->Frequency++;
		if (CodeLengths[j] > 15) j++; // Skip the extra bits
	}
	MakeCodeTree(CodeLengthCodes, Frequency, NCodeLengthCodes, 7);

	if (CodeLengthCodes[15].BitCount == 0) {
		NCodeLengthCodes--;
		if (CodeLengthCodes[1].BitCount == 0) {
			NCodeLengthCodes--;
			if (CodeLengthCodes[14].BitCount == 0) {
				NCodeLengthCodes--;
				if (CodeLengthCodes[2].BitCount == 0) {
					NCodeLengthCodes--;
					if (CodeLengthCodes[13].BitCount == 0) {
						NCodeLengthCodes--;
						if (CodeLengthCodes[3].BitCount == 0) {
							NCodeLengthCodes--;
							if (CodeLengthCodes[12].BitCount == 0) {
								NCodeLengthCodes--;
								if (CodeLengthCodes[4].BitCount == 0) {
									NCodeLengthCodes--;
									if (CodeLengthCodes[11].BitCount == 0) {
										NCodeLengthCodes--;
										if (CodeLengthCodes[5].BitCount == 0) {
											NCodeLengthCodes--;
											if (CodeLengthCodes[10].BitCount == 0) {
												NCodeLengthCodes--;
												if (CodeLengthCodes[6].BitCount == 0) {
													NCodeLengthCodes--;
													if (CodeLengthCodes[9].BitCount == 0) {
														NCodeLengthCodes--;
														if (CodeLengthCodes[7].BitCount == 0) {
															NCodeLengthCodes--;
															if (CodeLengthCodes[8].BitCount == 0) {
																NCodeLengthCodes--;
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// Output the header
	AddBits(NLiterals - 257, 5, Index, BitIndex);
	AddBits(NDistances - 1, 5, Index, BitIndex);
	AddBits(NCodeLengthCodes - 4, 4, Index, BitIndex);

	// Output the Code Length Tree
	j = 0;
	AddBits(CodeLengthCodes[16].BitCount, 3, Index, BitIndex); j++;
	AddBits(CodeLengthCodes[17].BitCount, 3, Index, BitIndex); j++;
	AddBits(CodeLengthCodes[18].BitCount, 3, Index, BitIndex); j++;
	AddBits(CodeLengthCodes[0].BitCount, 3, Index, BitIndex); j++;
	if (j < NCodeLengthCodes) {
		AddBits(CodeLengthCodes[8].BitCount, 3, Index, BitIndex); j++;
		if (j < NCodeLengthCodes) {
			AddBits(CodeLengthCodes[7].BitCount, 3, Index, BitIndex); j++;
			if (j < NCodeLengthCodes) {
				AddBits(CodeLengthCodes[9].BitCount, 3, Index, BitIndex); j++;
				if (j < NCodeLengthCodes) {
					AddBits(CodeLengthCodes[6].BitCount, 3, Index, BitIndex); j++;
					if (j < NCodeLengthCodes) {
						AddBits(CodeLengthCodes[10].BitCount, 3, Index, BitIndex); j++;
						if (j < NCodeLengthCodes) {
							AddBits(CodeLengthCodes[5].BitCount, 3, Index, BitIndex); j++;
							if (j < NCodeLengthCodes) {
								AddBits(CodeLengthCodes[11].BitCount, 3, Index, BitIndex); j++;
								if (j < NCodeLengthCodes) {
									AddBits(CodeLengthCodes[4].BitCount, 3, Index, BitIndex); j++;
									if (j < NCodeLengthCodes) {
										AddBits(CodeLengthCodes[12].BitCount, 3, Index, BitIndex); j++;
										if (j < NCodeLengthCodes) {
											AddBits(CodeLengthCodes[3].BitCount, 3, Index, BitIndex); j++;
											if (j < NCodeLengthCodes) {
												AddBits(CodeLengthCodes[13].BitCount, 3, Index, BitIndex); j++;
												if (j < NCodeLengthCodes) {
													AddBits(CodeLengthCodes[2].BitCount, 3, Index, BitIndex); j++;
													if (j < NCodeLengthCodes) {
														AddBits(CodeLengthCodes[14].BitCount, 3, Index, BitIndex); j++;
														if (j < NCodeLengthCodes) {
															AddBits(CodeLengthCodes[1].BitCount, 3, Index, BitIndex); j++;
															if (j < NCodeLengthCodes) {
																AddBits(CodeLengthCodes[15].BitCount, 3, Index, BitIndex);
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// Output the Code Lengths Stream
	for (j = 0; j < NCodeLengths; j++) {
		q = CodeLengths[j];
		AddBits(
			CodeLengthCodes[CodeLengths[j]].Code,
			CodeLengthCodes[CodeLengths[j]].BitCount,
			Index, BitIndex, true
		);
		switch (CodeLengths[j]) {
		case 16:
			j++;
			AddBits(CodeLengths[j], 2, Index, BitIndex);
			break;

		case 17:
			j++;
			AddBits(CodeLengths[j], 3, Index, BitIndex);
			break;

		case 18:
			j++;
			AddBits(CodeLengths[j], 7, Index, BitIndex);
			break;

		default:
			break;
		}
	}

	delete[] CodeLengths;
}

unsigned char* Deflate::RunDeflate(
	unsigned char* Buffer,
	unsigned* Length,
	unsigned       WindowSize
) {
	unsigned j;
	unsigned Index = 0;
	unsigned BitIndex = 0;
	unsigned length;
	unsigned distance;

	unsigned s1 = 1; // Adler32: sum of all bytes
	unsigned s2 = 0; // Adler32: sum of all s1

	unsigned char c;

	if (WindowSize > 0x8000) WindowSize = 0x8000;
	if (WindowSize < 0x8) WindowSize = 0x8;

	Deflate::WindowSize = WindowSize;

	CODE  ExtraBits;
	CODE* LiteralCodes;
	CODE* DistanceCodes;

	CODE_ELEMENT* Elements;
	unsigned      ElementCount;

	Deflate::Buffer = new unsigned char[1];
	Deflate::BufferSize = 1;
	Deflate::Length = 0;

	Elements = new CODE_ELEMENT[*Length];
	ElementCount = 0;

	c = 0x78; AddBinary(&c, 1); Index++; // Deflate; 32 kB window
	c = 0x9C; AddBinary(&c, 1); Index++; // Normal compression, no dictionary

	AddBits(1, 1, &Index, &BitIndex); // Final

	j = 0;
	while (j < *Length) {
		s1 = (s1 + Buffer[j]) % 65521;
		s2 = (s2 + s1) % 65521;
		if (FindLengthDistancePair(Buffer, j, *Length, &length, &distance)) {
			Elements[ElementCount].Distance = distance;
			Elements[ElementCount].Length = length;
			j += length;

		}
		else {
			Elements[ElementCount].Distance = 0;
			Elements[ElementCount].Literal = Buffer[j];
			j++;
		}
		ElementCount++;
	}

	LiteralCodes = new CODE[288];
	DistanceCodes = new CODE[32];
	if (ElementCount > FixedTreeLimit) {
		AddBits(2, 2, &Index, &BitIndex); // Dynamic  Tree
		DynamicCodes(
			Elements,
			ElementCount,
			LiteralCodes,
			DistanceCodes,
			&Index,
			&BitIndex
		);

	}
	else {
		AddBits(1, 2, &Index, &BitIndex); // Fixed Tree
		DefaultCodes(LiteralCodes, DistanceCodes);
	}

	for (j = 0; j < ElementCount; j++) {
		if (Elements[j].Distance) {
			length = LengthToLiteral(Elements[j].Length, &ExtraBits);
			AddBits(
				LiteralCodes[length].Code,
				LiteralCodes[length].BitCount,
				&Index, &BitIndex, true
			);
			AddBits(ExtraBits.Code, ExtraBits.BitCount, &Index, &BitIndex);
			distance = EncodeDistance(Elements[j].Distance, &ExtraBits);
			AddBits(
				DistanceCodes[distance].Code,
				DistanceCodes[distance].BitCount,
				&Index, &BitIndex, true
			);
			AddBits(ExtraBits.Code, ExtraBits.BitCount, &Index, &BitIndex);

		}
		else {
			AddBits(
				LiteralCodes[Elements[j].Literal].Code,
				LiteralCodes[Elements[j].Literal].BitCount,
				&Index, &BitIndex, true
			);
		}
	}

	// End of block
	AddBits(
		LiteralCodes[256].Code,
		LiteralCodes[256].BitCount,
		&Index, &BitIndex, true
	);

	delete[] LiteralCodes;
	delete[] DistanceCodes;
	delete[] Elements;

	if (BitIndex) {
		BitIndex = 0;
		Index++;
	}

	// Store Addler32
	c = s2 >> 8; AddBinary(&c, 1);
	c = s2 & 0xFF; AddBinary(&c, 1);
	c = s2 & 0xFF; AddBinary(&c, 1);
	c = s1 >> 8; AddBinary(&c, 1);
	c = s1 & 0xFF; AddBinary(&c, 1);

	*Length = Deflate::Length;
	Buffer = new unsigned char[*Length];
	for (j = 0; j < *Length; j++) {
		Buffer[j] = Deflate::Buffer[j];
	}

	delete[] Deflate::Buffer;
	Deflate::BufferSize = 0;
	Deflate::Length = 0;

	return Buffer;
}

void Deflate::AddToTree(
	HUFFMAN* Root,
	unsigned Symbol,
	unsigned Count,
	unsigned Code
) {
	HUFFMAN* Temp = Root;

	while (Count) {
		if ((Code >> (Count - 1)) & 1) {
			if (!(Temp->One)) Temp->One = new HUFFMAN;
			Temp = Temp->One;
		}
		else {
			if (!(Temp->Zero)) Temp->Zero = new HUFFMAN;
			Temp = Temp->Zero;
		}
		Count--;
	}

	Temp->Symbol = Symbol;
}

unsigned Deflate::DecodeSymbol(
	HUFFMAN* Tree,
	unsigned char* Buffer,
	unsigned* Index,
	unsigned* BitIndex
) {
	HUFFMAN* Temp = Tree;
	while (Temp->One || Temp->Zero) {
		if (GetBits(Buffer, 1, Index, BitIndex)) {
			Temp = Temp->One;
		}
		else {
			Temp = Temp->Zero;
		}
	}
	return Temp->Symbol;
}

void Deflate::DefaultTree(HUFFMAN* LiteralTree, HUFFMAN* DistanceTree) {
	unsigned j, c;

	j = 0;
	c = 0x30;
	while (j < 144) {
		AddToTree(LiteralTree, j, 8, c);
		j++;
		c++;
	}
	c = 0x190;
	while (j < 256) {
		AddToTree(LiteralTree, j, 9, c);
		j++;
		c++;
	}
	c = 0;
	while (j < 280) {
		AddToTree(LiteralTree, j, 7, c);
		j++;
		c++;
	}
	c = 0xC0;
	while (j < 288) {
		AddToTree(LiteralTree, j, 8, c);
		j++;
		c++;
	}

	for (j = 0; j < 32; j++) {
		AddToTree(DistanceTree, j, 5, j);
	}
}

void Deflate::BuildTree(HUFFMAN* Root, SYMBOL* Symbols, unsigned Count) {
	SYMBOL    Temp;
	unsigned  j, q, w;
	unsigned  MaxBitLength;
	unsigned* BitLengthCount;
	unsigned* StartCode;

	// Sort
	for (j = 0; j < Count; j++) {
		for (q = j + 1; q < Count; q++) {
			if (Symbols[j].BitCount > Symbols[q].BitCount) {
				Temp = Symbols[j];
				Symbols[j] = Symbols[q];
				Symbols[q] = Temp;
			}
			else if (Symbols[j].BitCount == Symbols[q].BitCount) {
				if (Symbols[j].Symbol > Symbols[q].Symbol) {
					Temp = Symbols[j];
					Symbols[j] = Symbols[q];
					Symbols[q] = Temp;
				}
			}
		}
	}

	// Count the number of codes for each code length
	MaxBitLength = Symbols[Count - 1].BitCount;
	BitLengthCount = new unsigned[MaxBitLength + 1];
	for (j = 0; j < MaxBitLength; j++) {
		BitLengthCount[j] = 0;
	}
	for (j = 0; j < Count; j++) {
		BitLengthCount[Symbols[j].BitCount]++;
	}
	BitLengthCount[0] = 0;

	// Find the numerical value of the start code for each set of bit lengths
	q = 0;
	StartCode = new unsigned[MaxBitLength + 1];
	StartCode[0] = 0;
	for (j = 1; j <= MaxBitLength; j++) {
		q = (q + BitLengthCount[j - 1]) << 1;
		StartCode[j] = q;
	}

	// Assign the codes to the symbols
	for (j = 0; Symbols[j].BitCount == 0; j++);
	w = 0;
	q = 0;
	for (; j < Count; j++) {
		if (Symbols[j].BitCount > w) {
			w = Symbols[j].BitCount;
			q = StartCode[w];
		}
		AddToTree(Root, Symbols[j].Symbol, Symbols[j].BitCount, q);
		q++;
	}

	delete[] BitLengthCount;
	delete[] StartCode;
}

void Deflate::DynamicTree(
	HUFFMAN* LiteralTree,
	HUFFMAN* DistanceTree,
	unsigned char* Buffer,
	unsigned* Index,
	unsigned* BitIndex
) {
	unsigned j, q;
	unsigned Repeat;
	unsigned Previous;
	unsigned NLiterals;
	unsigned NDistances;
	unsigned NCodeLengths;

	SYMBOL CodeLength[19];
	SYMBOL Literals[286];
	SYMBOL Distances[32];

	HUFFMAN* CodeTree = new HUFFMAN;

	CodeLength[0].Symbol = 16;
	CodeLength[1].Symbol = 17;
	CodeLength[2].Symbol = 18;
	CodeLength[3].Symbol = 0;
	CodeLength[4].Symbol = 8;
	CodeLength[5].Symbol = 7;
	CodeLength[6].Symbol = 9;
	CodeLength[7].Symbol = 6;
	CodeLength[8].Symbol = 10;
	CodeLength[9].Symbol = 5;
	CodeLength[10].Symbol = 11;
	CodeLength[11].Symbol = 4;
	CodeLength[12].Symbol = 12;
	CodeLength[13].Symbol = 3;
	CodeLength[14].Symbol = 13;
	CodeLength[15].Symbol = 2;
	CodeLength[16].Symbol = 14;
	CodeLength[17].Symbol = 1;
	CodeLength[18].Symbol = 15;

	NLiterals = GetBits(Buffer, 5, Index, BitIndex) + 257;
	NDistances = GetBits(Buffer, 5, Index, BitIndex) + 1;
	NCodeLengths = GetBits(Buffer, 4, Index, BitIndex) + 4;

	for (j = 0; j < NCodeLengths; j++) {
		CodeLength[j].BitCount = GetBits(Buffer, 3, Index, BitIndex);
	}
	for (; j < 19; j++) {
		CodeLength[j].BitCount = 0;
	}
	BuildTree(CodeTree, CodeLength, 19);

	for (j = 0; j < 286; j++) {
		Literals[j].Symbol = j;
		Literals[j].BitCount = 0;
	}
	Previous = j = 0;
	while (j < NLiterals) {
		q = DecodeSymbol(CodeTree, Buffer, Index, BitIndex);
		if (q < 16) {
			Literals[j++].BitCount = q;

		}
		else if (q == 16) {
			Repeat = GetBits(Buffer, 2, Index, BitIndex);
			Repeat += 3;
			while (Repeat) {
				Literals[j].BitCount = Previous;
				j++;
				Repeat--;
			}

		}
		else if (q == 17) {
			Repeat = GetBits(Buffer, 3, Index, BitIndex);
			Repeat += 3;
			while (Repeat) {
				Literals[j++].BitCount = 0;
				Repeat--;
			}

		}
		else if (q == 18) {
			Repeat = GetBits(Buffer, 7, Index, BitIndex);
			Repeat += 11;
			while (Repeat) {
				Literals[j++].BitCount = 0;
				Repeat--;
			}
		}
		Previous = Literals[j - 1].BitCount;
	}
	BuildTree(LiteralTree, Literals, 286);

	for (j = 0; j < 32; j++) {
		Distances[j].Symbol = j;
		Distances[j].BitCount = 0;
	}
	j = 0;
	while (j < NDistances) {
		q = DecodeSymbol(CodeTree, Buffer, Index, BitIndex);
		if (q < 16) {
			Distances[j++].BitCount = q;

		}
		else if (q == 16) {
			Repeat = GetBits(Buffer, 2, Index, BitIndex);
			Repeat += 3;
			while (Repeat) {
				Distances[j].BitCount = Previous;
				j++;
				Repeat--;
			}

		}
		else if (q == 17) {
			Repeat = GetBits(Buffer, 3, Index, BitIndex);
			Repeat += 3;
			while (Repeat) {
				Distances[j++].BitCount = 0;
				Repeat--;
			}

		}
		else if (q == 18) {
			Repeat = GetBits(Buffer, 7, Index, BitIndex);
			Repeat += 11;
			while (Repeat) {
				Distances[j++].BitCount = 0;
				Repeat--;
			}
		}
		Previous = Distances[j - 1].BitCount;
	}
	BuildTree(DistanceTree, Distances, 32);

	delete CodeTree;
}

unsigned Deflate::GetBits(
	unsigned char* Buffer,
	unsigned       Count,
	unsigned* Index,
	unsigned* BitIndex,
	bool           BitReverse
) {
	unsigned result;
	unsigned mask;

	mask = 0xFFFFFFFF;
	mask <<= Count;
	mask = ~mask;

	result = Buffer[(*Index)] |
		(Buffer[(*Index) + 1] << 8) |
		(Buffer[(*Index) + 2] << 16) |
		(Buffer[(*Index) + 3] << 24);

	result >>= *BitIndex;
	result &= mask;

	*BitIndex += Count;
	while (*BitIndex >= 8) {
		*Index += 1;
		*BitIndex -= 8;
	}

	if (BitReverse) {
		mask = result;
		result = 0;
		while (Count) {
			result <<= 1;
			result |= mask & 1;
			mask >>= 1;
			Count--;
		}
	}

	return result;
}

unsigned Deflate::LiteralToLength(
	unsigned       Literal,
	unsigned char* Buffer,
	unsigned* Index,
	unsigned* BitIndex
) {
	if (Literal < 265) { // 0 extra bits
		Literal -= 254;
	}
	else if (Literal < 269) { // 1 extra bit
		Literal -= 265;
		Literal <<= 1;
		Literal |= GetBits(Buffer, 1, Index, BitIndex);
		Literal += 11;
	}
	else if (Literal < 273) { // 2 extra bits
		Literal -= 269;
		Literal <<= 2;
		Literal |= GetBits(Buffer, 2, Index, BitIndex);
		Literal += 19;
	}
	else if (Literal < 277) { // 3 extra bits
		Literal -= 273;
		Literal <<= 3;
		Literal |= GetBits(Buffer, 3, Index, BitIndex);
		Literal += 35;
	}
	else if (Literal < 281) { // 4 extra bits
		Literal -= 277;
		Literal <<= 4;
		Literal |= GetBits(Buffer, 4, Index, BitIndex);
		Literal += 67;
	}
	else if (Literal < 285) { // 5 extra bits
		Literal -= 281;
		Literal <<= 5;
		Literal |= GetBits(Buffer, 5, Index, BitIndex);
		Literal += 131;
	}
	else {
		Literal = 258;
	}
	return Literal;
}

unsigned Deflate::DecodeDistance(
	unsigned       Distance,
	unsigned char* Buffer,
	unsigned* Index,
	unsigned* BitIndex
) {
	if (Distance < 4) { // 0  extra bits
		Distance += 1;
	}
	else if (Distance < 6) { // 1  extra bits
		Distance -= 4;
		Distance <<= 1;
		Distance |= GetBits(Buffer, 1, Index, BitIndex);
		Distance += 5;
	}
	else if (Distance < 8) { // 2  extra bits
		Distance -= 6;
		Distance <<= 2;
		Distance |= GetBits(Buffer, 2, Index, BitIndex);
		Distance += 9;
	}
	else if (Distance < 10) { // 3  extra bits
		Distance -= 8;
		Distance <<= 3;
		Distance |= GetBits(Buffer, 3, Index, BitIndex);
		Distance += 17;
	}
	else if (Distance < 12) { // 4  extra bits
		Distance -= 10;
		Distance <<= 4;
		Distance |= GetBits(Buffer, 4, Index, BitIndex);
		Distance += 33;
	}
	else if (Distance < 14) { // 5  extra bits
		Distance -= 12;
		Distance <<= 5;
		Distance |= GetBits(Buffer, 5, Index, BitIndex);
		Distance += 65;
	}
	else if (Distance < 16) { // 6  extra bits
		Distance -= 14;
		Distance <<= 6;
		Distance |= GetBits(Buffer, 6, Index, BitIndex);
		Distance += 129;
	}
	else if (Distance < 18) { // 7  extra bits
		Distance -= 16;
		Distance <<= 7;
		Distance |= GetBits(Buffer, 7, Index, BitIndex);
		Distance += 257;
	}
	else if (Distance < 20) { // 8  extra bits
		Distance -= 18;
		Distance <<= 8;
		Distance |= GetBits(Buffer, 8, Index, BitIndex);
		Distance += 513;
	}
	else if (Distance < 22) { // 9  extra bits
		Distance -= 20;
		Distance <<= 9;
		Distance |= GetBits(Buffer, 9, Index, BitIndex);
		Distance += 1025;
	}
	else if (Distance < 24) { // 10 extra bits
		Distance -= 22;
		Distance <<= 10;
		Distance |= GetBits(Buffer, 10, Index, BitIndex);
		Distance += 2049;
	}
	else if (Distance < 26) { // 11 extra bits
		Distance -= 24;
		Distance <<= 11;
		Distance |= GetBits(Buffer, 11, Index, BitIndex);
		Distance += 4097;
	}
	else if (Distance < 28) { // 12 extra bits
		Distance -= 26;
		Distance <<= 12;
		Distance |= GetBits(Buffer, 12, Index, BitIndex);
		Distance += 8193;
	}
	else if (Distance < 30) { // 13 extra bits
		Distance -= 28;
		Distance <<= 13;
		Distance |= GetBits(Buffer, 13, Index, BitIndex);
		Distance += 16385;
	}
	return Distance;
}

unsigned char* Deflate::Inflate(unsigned char* Buffer, unsigned* Length) {
	// char     CM;
	// char     CInfo;
	// char     FCheck;
	char     FDict;
	// char     FLevel;
	// unsigned DictID = 0;

	char     BFinal;
	char     BType;
	unsigned Literal; // or Length
	unsigned Distance;

	unsigned j;
	unsigned Index = 0;
	unsigned BitIndex = 0;

	unsigned char c;

	HUFFMAN* LiteralTree;
	HUFFMAN* DistanceTree;

	if (*Length < 2) return 0;

	Deflate::Buffer = new unsigned char[1];
	Deflate::BufferSize = 1;
	Deflate::Length = 0;

	// CM     =  Buffer[Index  ]       & 0x0F;
	// CInfo  = (Buffer[Index++] >> 4) & 0x0F;

	// FCheck =  Buffer[Index  ]       & 0x1F;
	FDict = (Buffer[Index] >> 5) & 0x01;
	// FLevel = (Buffer[Index++] >> 6) & 0x03;

	if (FDict) {
		return 0;
		// DictID = (Buffer[Index  ] << 24) |
		//          (Buffer[Index+1] << 16) |
		//          (Buffer[Index+2] <<  8) |
		//          (Buffer[Index+3]); Index += 4;
	}

	while (Index < *Length) {
		BFinal = GetBits(Buffer, 1, &Index, &BitIndex);
		BType = GetBits(Buffer, 2, &Index, &BitIndex);

		if (BType == 0) { // No compression
			if (BitIndex) {
				Index++;
				BitIndex = 0;
			}
			Distance = (Buffer[Index] << 8) |
				(Buffer[Index + 1]); Index += 4; // Skip NLen
			AddBinary(Buffer + Index, Distance);
			Index += Distance;

		}
		else { // Compression
			LiteralTree = new HUFFMAN;
			DistanceTree = new HUFFMAN;
			if (BType == 2) { // Dynamic Huffman Codes
				DynamicTree(LiteralTree, DistanceTree, Buffer, &Index, &BitIndex);
			}
			else {
				DefaultTree(LiteralTree, DistanceTree);
			}

			while (Index < *Length) {
				Literal = DecodeSymbol(LiteralTree, Buffer, &Index, &BitIndex);
				if (Literal < 0x100) {
					AddBinary((unsigned char*)&Literal, 1);

				}
				else if (Literal == 0x100) {
					break;

				}
				else {
					Literal = LiteralToLength(Literal, Buffer, &Index, &BitIndex);
					Distance = DecodeSymbol(DistanceTree, Buffer, &Index, &BitIndex);
					Distance = DecodeDistance(Distance, Buffer, &Index, &BitIndex);
					while (Literal) { // Use Literal as Length
						c = Deflate::Buffer[(int)(Deflate::Length)-Distance];
						AddBinary(&c, 1);
						Literal--;
					}
				}
			}
			delete LiteralTree;
			delete DistanceTree;
		}

		if (BFinal) break;
	}

	*Length = Deflate::Length;
	Buffer = new unsigned char[*Length];
	for (j = 0; j < *Length; j++) {
		Buffer[j] = Deflate::Buffer[j];
	}

	delete[] Deflate::Buffer;
	Deflate::BufferSize = 0;
	Deflate::Length = 0;

	return Buffer;
}
