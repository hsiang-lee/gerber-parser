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

#include "pdfFontMetrics.h"

static unsigned TheGlyphCount;

pdfFontMetrics::pdfFontMetrics() {
	FontName = 0;
	Metrics = 0;
	Characters = 0;

	TheGlyphCount = 0;
}

pdfFontMetrics::~pdfFontMetrics() {
	if (FontName) delete[] FontName;
	if (Metrics) delete   Metrics;
	if (Characters) delete[] Characters;
}

pdfFontMetrics::BOX::BOX() {
	Left = Right = Top = Bottom = 0;
}

pdfFontMetrics::KERNING::KERNING() {
	Name = 0;
	Offset = 0;
	Left = 0;
	Right = 0;
	Parent = 0;
	Red = true;
}

pdfFontMetrics::KERNING::~KERNING() {
	if (Name) delete[] Name;
	if (Left) delete   Left;
	if (Right) delete   Right;
}

void pdfFontMetrics::Restructure(METRIC* Container, KERNING* N) {
	KERNING* P;  // Parent
	KERNING* G;  // Grandparent
	KERNING* U;  // Uncle
	KERNING** GG; // The pointer to the grandparent
	KERNING* a;  // Leaves
	KERNING* b;
	KERNING* c;

	P = N->Parent;
	G = P->Parent;

	// Find the pointer to the grandparent
	U = G->Parent;
	if (U) {
		if (G == U->Left) {
			GG = &(U->Left);
		}
		else {
			GG = &(U->Right);
		}
	}
	else { // No great grandparent
		GG = &(Container->Kerning);
	}

	if (P == G->Left) {
		U = G->Right;
	}
	else {
		U = G->Left;
	}

	if (N == P->Left) {
		if (P == G->Left) {
			a = N->Left;
			b = N->Right;
			c = P->Right;
			*GG = P;
			P->Parent = G->Parent;
			P->Left = N;
			P->Right = G;
			N->Parent = P;
			N->Left = a;
			N->Right = b;
			G->Parent = P;
			G->Left = c;
			G->Right = U;
			if (a) a->Parent = N;
			if (b) b->Parent = N;
			if (c) c->Parent = G;
			P->Red = false;
			N->Red = true;
			G->Red = true;

		}
		else { // P is right child of G
			a = N->Left;
			b = N->Right;
			c = P->Right;
			*GG = N;
			N->Parent = G->Parent;
			N->Left = G;
			N->Right = P;
			P->Parent = N;
			P->Left = b;
			P->Right = c;
			G->Parent = N;
			G->Left = U;
			G->Right = a;
			if (a) a->Parent = G;
			if (b) b->Parent = P;
			if (c) c->Parent = P;
			N->Red = false;
			G->Red = true;
			P->Red = true;
		}

	}
	else { // N is right child of P
		if (P == G->Right) {
			a = P->Left;
			b = N->Left;
			c = N->Right;
			*GG = P;
			P->Parent = G->Parent;
			P->Left = G;
			P->Right = N;
			N->Parent = P;
			N->Left = b;
			N->Right = c;
			G->Parent = P;
			G->Left = U;
			G->Right = a;
			if (a) a->Parent = G;
			if (b) b->Parent = N;
			if (c) c->Parent = N;
			P->Red = false;
			G->Red = true;
			N->Red = true;

		}
		else { // P is left child of G
			a = P->Left;
			b = N->Left;
			c = N->Right;
			*GG = N;
			N->Parent = G->Parent;
			N->Left = P;
			N->Right = G;
			P->Parent = N;
			P->Left = a;
			P->Right = b;
			G->Parent = N;
			G->Left = c;
			G->Right = U;
			if (a) a->Parent = P;
			if (b) b->Parent = P;
			if (c) c->Parent = G;
			N->Red = false;
			P->Red = true;
			G->Red = true;
		}
	}
}

void pdfFontMetrics::FixDoubleRed(METRIC* Container, KERNING* N) {
	KERNING* P; // Parent
	KERNING* G; // Grandparent
	KERNING* U; // Uncle

	if (N->Red) {
		P = N->Parent;
		if (P) {
			if (P->Red) {
				G = P->Parent;
				if (G) {
					if (P == G->Left) {
						U = G->Right;
					}
					else {
						U = G->Left;
					}
					if (U && U->Red) {
						G->Red = true;
						P->Red = false;
						U->Red = false;
						FixDoubleRed(Container, G);
					}
					else {
						Restructure(Container, N);
					}
				}
				else { // No grandparent
					P->Red = false;
				}
			}
		}
		else { // No parent
			N->Red = false;
		}
	}
}

bool pdfFontMetrics::Insert(METRIC* Container, KERNING* Node) {
	KERNING* Temp;

	if (!(Container->Kerning)) {
		Node->Red = false;
		Container->Kerning = Node;
		return true;
	}

	Node->Left = 0;
	Node->Right = 0;
	Node->Red = true; // Insert a red node to maintain black depth

	Temp = Container->Kerning;
	while (Temp) {
		switch (Compare(Node->Name, Temp->Name)) {
		case 0: // The entry already exists
			return false;

		case -1:
			if (Temp->Left) {
				Temp = Temp->Left;
			}
			else {
				Node->Parent = Temp;
				Temp->Left = Node;
				FixDoubleRed(Container, Node);
				return true;
			}
			break;

		case 1:
			if (Temp->Right) {
				Temp = Temp->Right;
			}
			else {
				Node->Parent = Temp;
				Temp->Right = Node;
				FixDoubleRed(Container, Node);
				return true;
			}
			break;

		default:
			break;
		}
	}
	return false;
}

pdfFontMetrics::METRIC::METRIC() {
	Code = -1;
	Name = 0;
	Advance = 0;
	Kerning = 0;
	Left = 0;
	Right = 0;
	Parent = 0;
	Red = true;
}

pdfFontMetrics::METRIC::~METRIC() {
	if (Name) delete[] Name;
	if (Kerning) delete   Kerning;
	if (Left) delete   Left;
	if (Right) delete   Right;

	TheGlyphCount--;
}

void pdfFontMetrics::Restructure(METRIC* N) {
	METRIC* P;  // Parent
	METRIC* G;  // Grandparent
	METRIC* U;  // Uncle
	METRIC** GG; // The pointer to the grandparent
	METRIC* a;  // Leaves
	METRIC* b;
	METRIC* c;

	P = N->Parent;
	G = P->Parent;

	// Find the pointer to the grandparent
	U = G->Parent;
	if (U) {
		if (G == U->Left) {
			GG = &(U->Left);
		}
		else {
			GG = &(U->Right);
		}
	}
	else { // No great grandparent
		GG = &Metrics;
	}

	if (P == G->Left) {
		U = G->Right;
	}
	else {
		U = G->Left;
	}

	if (N == P->Left) {
		if (P == G->Left) {
			a = N->Left;
			b = N->Right;
			c = P->Right;
			*GG = P;
			P->Parent = G->Parent;
			P->Left = N;
			P->Right = G;
			N->Parent = P;
			N->Left = a;
			N->Right = b;
			G->Parent = P;
			G->Left = c;
			G->Right = U;
			if (a) a->Parent = N;
			if (b) b->Parent = N;
			if (c) c->Parent = G;
			P->Red = false;
			N->Red = true;
			G->Red = true;

		}
		else { // P is right child of G
			a = N->Left;
			b = N->Right;
			c = P->Right;
			*GG = N;
			N->Parent = G->Parent;
			N->Left = G;
			N->Right = P;
			P->Parent = N;
			P->Left = b;
			P->Right = c;
			G->Parent = N;
			G->Left = U;
			G->Right = a;
			if (a) a->Parent = G;
			if (b) b->Parent = P;
			if (c) c->Parent = P;
			N->Red = false;
			G->Red = true;
			P->Red = true;
		}

	}
	else { // N is right child of P
		if (P == G->Right) {
			a = P->Left;
			b = N->Left;
			c = N->Right;
			*GG = P;
			P->Parent = G->Parent;
			P->Left = G;
			P->Right = N;
			N->Parent = P;
			N->Left = b;
			N->Right = c;
			G->Parent = P;
			G->Left = U;
			G->Right = a;
			if (a) a->Parent = G;
			if (b) b->Parent = N;
			if (c) c->Parent = N;
			P->Red = false;
			G->Red = true;
			N->Red = true;

		}
		else { // P is left child of G
			a = P->Left;
			b = N->Left;
			c = N->Right;
			*GG = N;
			N->Parent = G->Parent;
			N->Left = P;
			N->Right = G;
			P->Parent = N;
			P->Left = a;
			P->Right = b;
			G->Parent = N;
			G->Left = c;
			G->Right = U;
			if (a) a->Parent = P;
			if (b) b->Parent = P;
			if (c) c->Parent = G;
			N->Red = false;
			P->Red = true;
			G->Red = true;
		}
	}
}

void pdfFontMetrics::FixDoubleRed(METRIC* N) {
	METRIC* P; // Parent
	METRIC* G; // Grandparent
	METRIC* U; // Uncle

	if (N->Red) {
		P = N->Parent;
		if (P) {
			if (P->Red) {
				G = P->Parent;
				if (G) {
					if (P == G->Left) {
						U = G->Right;
					}
					else {
						U = G->Left;
					}
					if (U && U->Red) {
						G->Red = true;
						P->Red = false;
						U->Red = false;
						FixDoubleRed(G);
					}
					else {
						Restructure(N);
					}
				}
				else { // No grandparent
					P->Red = false;
				}
			}
		}
		else { // No parent
			N->Red = false;
		}
	}
}

bool pdfFontMetrics::Insert(METRIC* Node) {
	METRIC* Temp;

	if (!Metrics) {
		Node->Red = false;
		Metrics = Node;
		TheGlyphCount++;
		return true;
	}

	Node->Left = 0;
	Node->Right = 0;
	Node->Red = true; // Insert a red node to maintain black depth

	Temp = Metrics;
	while (Temp) {
		switch (Compare(Node->Name, Temp->Name)) {
		case 0: // The entry already exists
			return false;

		case -1:
			if (Temp->Left) {
				Temp = Temp->Left;
			}
			else {
				Node->Parent = Temp;
				Temp->Left = Node;
				FixDoubleRed(Node);
				TheGlyphCount++;
				return true;
			}
			break;

		case 1:
			if (Temp->Right) {
				Temp = Temp->Right;
			}
			else {
				Node->Parent = Temp;
				Temp->Right = Node;
				FixDoubleRed(Node);
				TheGlyphCount++;
				return true;
			}
			break;

		default:
			break;
		}
	}
	return false;
}

pdfFontMetrics::METRIC* pdfFontMetrics::FindMetric(const char* Name) {
	int     c;
	METRIC* Temp;

	Temp = Metrics;
	while (Temp) {
		c = Compare(Name, Temp->Name);
		if (c < 0) {
			Temp = Temp->Left;
		}
		else if (c > 0) {
			Temp = Temp->Right;
		}
		else {
			break;
		}
	}

	return Temp;
}

int pdfFontMetrics::Compare(const char* s1, const char* s2) {
	int j;

	for (j = 0; s1[j] && s2[j]; j++) {
		if (s1[j] < s2[j]) return -1;
		if (s1[j] > s2[j]) return  1;
	}

	if (s1[j]) return  1;
	if (s2[j]) return -1;

	return 0;
}

bool pdfFontMetrics::GetFontMetrics() {
	int    j, l;
	char   String[0x1000];
	double d;

	if (!Parser->GetKey(String)) {
		printf("Error: Key expected\n");
		return false;
	}
	if (Compare(String, "StartFontMetrics")) {
		printf("Error: \"StartFontMetrics\" expected\n");
		return false;
	}

	Parser->GetNumber(&d);
	Parser->GetEndOfLine();

	while (Parser->GetKey(String)) {
		if (!Compare(String, "EndFontMetrics")) {
			return true;

		}
		else if (!Compare(String, "FontName")) {
			if (!Parser->GetString(String)) {
				printf("Metrics Error: Font Name expected\n");
				return false;
			}
			for (l = 0; String[l]; l++);
			if (FontName) delete[] FontName;
			FontName = new char[l + 1];
			for (j = 0; j < l; j++) {
				FontName[j] = String[j];
			}
			FontName[j] = 0;
			Parser->GetEndOfLine();

		}
		else if (!Compare(String, "FontBBox")) {
			if (!Parser->GetNumber(&d)) {
				printf("Metrics Error: Number expected\n");
				return false;
			}
			Box.Left = d;
			if (!Parser->GetNumber(&d)) {
				printf("Metrics Error: Number expected\n");
				return false;
			}
			Box.Bottom = d;
			if (!Parser->GetNumber(&d)) {
				printf("Metrics Error: Number expected\n");
				return false;
			}
			Box.Right = d;
			if (!Parser->GetNumber(&d)) {
				printf("Metrics Error: Number expected\n");
				return false;
			}
			Box.Top = d;
			Parser->GetEndOfLine();

		}
		else if (!Compare(String, "StartCharMetrics")) {
			if (!GetCharMetrics()) {
				printf("Metrics Error: Char Metrics Error\n");
				return false;
			}

		}
		else if (!Compare(String, "StartKernData")) {
			if (!GetKernData()) return false;

		}
		else { // Ignore others
			if (!Parser->Ignore(String)) return false;
		}
	}

	printf("Metrics Error: \"EndFontMetrics\" expected\n");
	return false;
}

bool pdfFontMetrics::GetCharMetrics() {
	int     n;
	int     count;
	char    Key[0x100];
	char    Name[0x100];
	double  d;
	METRIC* Temp;
	METRIC* Temp2;

	Parser->GetWhiteSpace();
	Parser->GetInteger(&count);
	Parser->GetEndOfLine();

	if (!Characters) {
		Characters = new METRIC * [0x100];
		for (n = 0; n < 0x100; n++) {
			Characters[n] = 0;
		}
	}

	Temp = new METRIC;
	while (Parser->GetKey(Key)) {
		if (!Compare(Key, "EndCharMetrics")) {
			Parser->GetEndOfLine();
			delete Temp;
			return true;

		}
		else if (!Compare(Key, "C")) {
			if (!Parser->GetInteger(&n)) return false;
			Temp->Code = n;

		}
		else if (!Compare(Key, "CH")) {
			unsigned n;
			if (!Parser->GetHexInteger(&n)) return false;
			Temp->Code = n;

		}
		else if (!Compare(Key, "WX")) {
			if (!Parser->GetNumber(&d)) return false;
			Temp->Advance = d;

		}
		else if (!Compare(Key, "N")) {
			if (!Parser->GetName(Name)) return false;
			if (Temp->Name) delete[] Temp->Name;
			for (n = 0; Name[n]; n++);
			Temp->Name = new char[n + 1];
			for (n = 0; Name[n]; n++) {
				Temp->Name[n] = Name[n];
			}
			Temp->Name[n] = 0;

		}
		else if (!Compare(Key, "B")) {
			if (!Parser->GetNumber(&d)) return false;
			Temp->Box.Left = d;
			if (!Parser->GetNumber(&d)) return false;
			Temp->Box.Bottom = d;
			if (!Parser->GetNumber(&d)) return false;
			Temp->Box.Right = d;
			if (!Parser->GetNumber(&d)) return false;
			Temp->Box.Top = d;
		}

		if (!Parser->IgnoreCharSection()) return false;

		if (Parser->GetEndOfLine()) {
			if (Insert(Temp)) {
				if (Temp->Code >= 0x00 && Temp->Code <= 0xFF) {
					Characters[Temp->Code] = Temp;
				}
			}
			else {
				Temp2 = FindMetric(Temp->Name);
				if (Temp2) {
					if (Temp->Code >= 0x00 && Temp->Code <= 0xFF) {
						Characters[Temp->Code] = Temp2;
					}
				}
				delete Temp;
			}
			Temp = new METRIC;
		}
	}

	return false;
}

bool pdfFontMetrics::GetKernData() {
	char Key[0x100];

	if (!Parser->GetEndOfLine()) return false;

	while (Parser->GetKey(Key)) {
		if (!Compare(Key, "EndKernData")) {
			Parser->GetEndOfLine();
			return true;

		}
		else if (!Compare(Key, "StartKernPairs")) {
			if (!GetKernPairs()) return false;

		}
		else { // Ignore others
			if (!Parser->Ignore(Key)) return false;
		}
	}

	printf("Kerning Error: \"EndKernData\" expected\n");
	return false;
}

void pdfFontMetrics::AddKerning(
	const char* Name1,
	const char* Name2,
	double      Offset
) {
	int      n;
	KERNING* Temp;
	METRIC* Metric;

	Metric = FindMetric(Name1);
	if (Metric) {
		Temp = new KERNING;
		for (n = 0; Name2[n]; n++);
		Temp->Name = new char[n + 1];
		for (n = 0; Name2[n]; n++) {
			Temp->Name[n] = Name2[n];
		}
		Temp->Name[n] = 0;
		Temp->Offset = Offset;
		if (!Insert(Metric, Temp)) {
			//   printf("Duplicate Kerning: %s -> %s\n", Name1, Name2);
			delete Temp;
		}
	}
	else {
		//  printf("Name1 not found: %s -> %s\n", Name1, Name2);
	}

	if (Code(Name2) < 0) {
		//  printf("Name2 not found: %s -> %s\n", Name1, Name2);
	}
}

bool pdfFontMetrics::GetKernPairs() {
	int      count;
	char     Key[0x100];
	char     Name1[0x100];
	char     Name2[0x100];
	double   d;

	Parser->GetWhiteSpace();
	Parser->GetInteger(&count);
	Parser->GetEndOfLine();

	while (Parser->GetKey(Key)) {
		if (!Compare(Key, "EndKernPairs")) {
			Parser->GetEndOfLine();
			return true;

		}
		else if (!Compare(Key, "KPX")) {
			if (!Parser->GetName(Name1)) {
				printf("Kerning Error: Name expected\n");
				return false;
			}
			if (!Parser->GetName(Name2)) {
				printf("Kerning Error: Name expected\n");
				return false;
			}
			if (!Parser->GetNumber(&d)) {
				printf("Kerning Error: Number expected\n");
				return false;
			}
			if (!Parser->GetEndOfLine()) {
				printf("Kerning Error: End-of-line expected\n");
				return false;
			}
			AddKerning(Name1, Name2, d);

		}
		else {
			if (!Parser->Ignore(Key)) return false;
		}
	}

	printf("Kerning Error: \"EndKernPairs\" expected\n");
	return false;
}

bool pdfFontMetrics::LoadAFM(afmParser* Parser) {
	pdfFontMetrics::Parser = Parser;
	return GetFontMetrics();
}

bool pdfFontMetrics::LoadAFM(const char* FileName) {
	bool b;

	if (TheParser.Open(FileName)) {
		b = LoadAFM(&TheParser);
		TheParser.Close();
		return b;
	}

	return false;
}

void pdfFontMetrics::SetEncoding(int Code, const char* Name) {
	int j;

	METRIC* Temp = FindMetric(Name);

	if (Code >= 0x00 && Code <= 0xFF) {
		if (!Characters) {
			Characters = new METRIC * [0x100];
			for (j = 0; j < 0x100; j++) {
				Characters[j] = 0;
			}
		}
		if (Characters[Code]) {
			Characters[Code]->Code = -1;
		}
		Characters[Code] = Temp;
	}

	if (Temp) Temp->Code = Code;
}

void pdfFontMetrics::SetWinANSI() {
	SetEncoding(0000, "");
	SetEncoding(0001, "dotlessi");
	SetEncoding(0002, "caron");
	SetEncoding(0003, "");
	SetEncoding(0004, "");
	SetEncoding(0005, "");
	SetEncoding(0006, "");
	SetEncoding(0007, "");

	SetEncoding(0010, "");
	SetEncoding(0011, "");
	SetEncoding(0012, "");
	SetEncoding(0013, "");
	SetEncoding(0014, "");
	SetEncoding(0015, "");
	SetEncoding(0016, "");
	SetEncoding(0017, "");

	SetEncoding(0020, "");
	SetEncoding(0021, "");
	SetEncoding(0022, "");
	SetEncoding(0023, "");
	SetEncoding(0024, "");
	SetEncoding(0025, "");
	SetEncoding(0026, "");
	SetEncoding(0027, "");

	SetEncoding(0030, "");
	SetEncoding(0031, "");
	SetEncoding(0032, "");
	SetEncoding(0033, "");
	SetEncoding(0034, "");
	SetEncoding(0035, "");
	SetEncoding(0036, "");
	SetEncoding(0037, "");

	SetEncoding(0040, "space");
	SetEncoding(0041, "exclam");
	SetEncoding(0042, "quotedbl");
	SetEncoding(0043, "numbersign");
	SetEncoding(0044, "dollar");
	SetEncoding(0045, "percent");
	SetEncoding(0046, "ampersand");
	SetEncoding(0047, "quotesingle");

	SetEncoding(0050, "parenleft");
	SetEncoding(0051, "parenright");
	SetEncoding(0052, "asterisk");
	SetEncoding(0053, "plus");
	SetEncoding(0054, "comma");
	SetEncoding(0055, "hyphen");
	SetEncoding(0056, "period");
	SetEncoding(0057, "slash");

	SetEncoding(0060, "zero");
	SetEncoding(0061, "one");
	SetEncoding(0062, "two");
	SetEncoding(0063, "three");
	SetEncoding(0064, "four");
	SetEncoding(0065, "five");
	SetEncoding(0066, "six");
	SetEncoding(0067, "seven");

	SetEncoding(0070, "eight");
	SetEncoding(0071, "nine");
	SetEncoding(0072, "colon");
	SetEncoding(0073, "semicolon");
	SetEncoding(0074, "less");
	SetEncoding(0075, "equal");
	SetEncoding(0076, "greater");
	SetEncoding(0077, "question");

	SetEncoding(0100, "at");
	SetEncoding(0101, "A");
	SetEncoding(0102, "B");
	SetEncoding(0103, "C");
	SetEncoding(0104, "D");
	SetEncoding(0105, "E");
	SetEncoding(0106, "F");
	SetEncoding(0107, "G");

	SetEncoding(0110, "H");
	SetEncoding(0111, "I");
	SetEncoding(0112, "J");
	SetEncoding(0113, "K");
	SetEncoding(0114, "L");
	SetEncoding(0115, "M");
	SetEncoding(0116, "N");
	SetEncoding(0117, "O");

	SetEncoding(0120, "P");
	SetEncoding(0121, "Q");
	SetEncoding(0122, "R");
	SetEncoding(0123, "S");
	SetEncoding(0124, "T");
	SetEncoding(0125, "U");
	SetEncoding(0126, "V");
	SetEncoding(0127, "W");

	SetEncoding(0130, "X");
	SetEncoding(0131, "Y");
	SetEncoding(0132, "Z");
	SetEncoding(0133, "bracketleft");
	SetEncoding(0134, "backslash");
	SetEncoding(0135, "bracketright");
	SetEncoding(0136, "asciicircum");
	SetEncoding(0137, "underscore");

	SetEncoding(0140, "grave");
	SetEncoding(0141, "a");
	SetEncoding(0142, "b");
	SetEncoding(0143, "c");
	SetEncoding(0144, "d");
	SetEncoding(0145, "e");
	SetEncoding(0146, "f");
	SetEncoding(0147, "g");

	SetEncoding(0150, "h");
	SetEncoding(0151, "i");
	SetEncoding(0152, "j");
	SetEncoding(0153, "k");
	SetEncoding(0154, "l");
	SetEncoding(0155, "m");
	SetEncoding(0156, "n");
	SetEncoding(0157, "o");

	SetEncoding(0160, "p");
	SetEncoding(0161, "q");
	SetEncoding(0162, "r");
	SetEncoding(0163, "s");
	SetEncoding(0164, "t");
	SetEncoding(0165, "u");
	SetEncoding(0166, "v");
	SetEncoding(0167, "w");

	SetEncoding(0170, "x");
	SetEncoding(0171, "y");
	SetEncoding(0172, "z");
	SetEncoding(0173, "braceleft");
	SetEncoding(0174, "bar");
	SetEncoding(0175, "braceright");
	SetEncoding(0176, "asciitilde");
	SetEncoding(0177, "");

	SetEncoding(0200, "Euro");
	SetEncoding(0201, "");
	SetEncoding(0202, "quotesinglbase");
	SetEncoding(0203, "florin");
	SetEncoding(0204, "quotedblbase");
	SetEncoding(0205, "ellipsis");
	SetEncoding(0206, "dagger");
	SetEncoding(0207, "daggerdbl");

	SetEncoding(0210, "circumflex");
	SetEncoding(0211, "perthousand");
	SetEncoding(0212, "Scaron");
	SetEncoding(0213, "guilsinglleft");
	SetEncoding(0214, "OE");
	SetEncoding(0215, "");
	SetEncoding(0216, "Zcaron");
	SetEncoding(0217, "");

	SetEncoding(0220, "");
	SetEncoding(0221, "quoteleft");
	SetEncoding(0222, "quoteright");
	SetEncoding(0223, "quotedblleft");
	SetEncoding(0224, "quotedblright");
	SetEncoding(0225, "bullet");
	SetEncoding(0226, "endash");
	SetEncoding(0227, "emdash");

	SetEncoding(0230, "tilde");
	SetEncoding(0231, "trademark");
	SetEncoding(0232, "scaron");
	SetEncoding(0233, "guilsinglright");
	SetEncoding(0234, "oe");
	SetEncoding(0235, "");
	SetEncoding(0236, "zcaron");
	SetEncoding(0237, "Ydieresis");

	SetEncoding(0240, "");
	SetEncoding(0241, "exclamdown");
	SetEncoding(0242, "cent");
	SetEncoding(0243, "sterling");
	SetEncoding(0244, "currency");
	SetEncoding(0245, "yen");
	SetEncoding(0246, "brokenbar");
	SetEncoding(0247, "section");

	SetEncoding(0250, "dieresis");
	SetEncoding(0251, "copyright");
	SetEncoding(0252, "ordfeminine");
	SetEncoding(0253, "guillemotleft");
	SetEncoding(0254, "logicalnot");
	SetEncoding(0255, "");
	SetEncoding(0256, "registered");
	SetEncoding(0257, "macron");

	SetEncoding(0260, "degree");
	SetEncoding(0261, "plusminus");
	SetEncoding(0262, "twosuperior");
	SetEncoding(0263, "threesuperior");
	SetEncoding(0264, "acute");
	SetEncoding(0265, "mu");
	SetEncoding(0266, "paragraph");
	SetEncoding(0267, "periodcentered");

	SetEncoding(0270, "cedilla");
	SetEncoding(0271, "onesuperior");
	SetEncoding(0272, "ordmasculine");
	SetEncoding(0273, "guillemotright");
	SetEncoding(0274, "onequarter");
	SetEncoding(0275, "onehalf");
	SetEncoding(0276, "threequarters");
	SetEncoding(0277, "questiondown");

	SetEncoding(0300, "Agrave");
	SetEncoding(0301, "Aacute");
	SetEncoding(0302, "Acircumflex");
	SetEncoding(0303, "Atilde");
	SetEncoding(0304, "Adieresis");
	SetEncoding(0305, "Aring");
	SetEncoding(0306, "AE");
	SetEncoding(0307, "Ccedilla");

	SetEncoding(0310, "Egrave");
	SetEncoding(0311, "Eacute");
	SetEncoding(0312, "Ecircumflex");
	SetEncoding(0313, "Edieresis");
	SetEncoding(0314, "Igrave");
	SetEncoding(0315, "Iacute");
	SetEncoding(0316, "Icircumflex");
	SetEncoding(0317, "Idieresis");

	SetEncoding(0320, "Eth");
	SetEncoding(0321, "Ntilde");
	SetEncoding(0322, "Ograve");
	SetEncoding(0323, "Oacute");
	SetEncoding(0324, "Ocircumflex");
	SetEncoding(0325, "Otilde");
	SetEncoding(0326, "Odieresis");
	SetEncoding(0327, "multiply");

	SetEncoding(0330, "Oslash");
	SetEncoding(0331, "Ugrave");
	SetEncoding(0332, "Uacute");
	SetEncoding(0333, "Ucircumflex");
	SetEncoding(0334, "Udieresis");
	SetEncoding(0335, "Yacute");
	SetEncoding(0336, "Thorn");
	SetEncoding(0337, "germandbls");

	SetEncoding(0340, "agrave");
	SetEncoding(0341, "aacute");
	SetEncoding(0342, "acircumflex");
	SetEncoding(0343, "atilde");
	SetEncoding(0344, "adieresis");
	SetEncoding(0345, "aring");
	SetEncoding(0346, "ae");
	SetEncoding(0347, "ccedilla");

	SetEncoding(0350, "egrave");
	SetEncoding(0351, "eacute");
	SetEncoding(0352, "ecircumflex");
	SetEncoding(0353, "edieresis");
	SetEncoding(0354, "igrave");
	SetEncoding(0355, "iacute");
	SetEncoding(0356, "icircumflex");
	SetEncoding(0357, "idieresis");

	SetEncoding(0360, "eth");
	SetEncoding(0361, "ntilde");
	SetEncoding(0362, "ograve");
	SetEncoding(0363, "oacute");
	SetEncoding(0364, "ocircumflex");
	SetEncoding(0365, "otilde");
	SetEncoding(0366, "odieresis");
	SetEncoding(0367, "divide");

	SetEncoding(0370, "oslash");
	SetEncoding(0371, "ugrave");
	SetEncoding(0372, "uacute");
	SetEncoding(0373, "ucircumflex");
	SetEncoding(0374, "udieresis");
	SetEncoding(0375, "yacute");
	SetEncoding(0376, "thorn");
	SetEncoding(0377, "ydieresis");
}

bool pdfFontMetrics::HasGlyph(char c) {
	if (!Characters) return false;
	return Characters[(unsigned char)c];
}

unsigned GlyphCount() {
	return TheGlyphCount;
}

int pdfFontMetrics::Code(const char* Name) {
	METRIC* Temp = FindMetric(Name);
	if (Temp) return Temp->Code;
	return -2;
}

void pdfFontMetrics::Name(char c, char* Name) {
	if (!Characters) return;
	METRIC* Metrics = Characters[(unsigned char)c];
	if (!Metrics) return;

	int j;
	for (j = 0; Metrics->Name[j]; j++) {
		Name[j] = Metrics->Name[j];
	}
	Name[j] = 0;
}

double pdfFontMetrics::Left(char c) {
	if (!Characters) return 0;
	METRIC* Metrics = Characters[(unsigned char)c];
	if (!Metrics) return 0;
	return Metrics->Box.Left;
}

double pdfFontMetrics::Bottom(char c) {
	if (!Characters) return 0;
	METRIC* Metrics = Characters[(unsigned char)c];
	if (!Metrics) return 0;
	return Metrics->Box.Bottom;
}

double pdfFontMetrics::Right(char c) {
	if (!Characters) return 0;
	METRIC* Metrics = Characters[(unsigned char)c];
	if (!Metrics) return 0;
	return Metrics->Box.Right;
}

double pdfFontMetrics::Top(char c) {
	if (!Characters) return 0;
	METRIC* Metrics = Characters[(unsigned char)c];
	if (!Metrics) return 0;
	return Metrics->Box.Top;
}

double pdfFontMetrics::Width(char c) {
	if (!Characters) return 0;
	METRIC* Metrics = Characters[(unsigned char)c];
	if (!Metrics) return 0;
	return Metrics->Box.Right - Metrics->Box.Left;
}

double pdfFontMetrics::Height(char c) {
	if (!Characters) return 0;
	METRIC* Metrics = Characters[(unsigned char)c];
	if (!Metrics) return 0;
	return Metrics->Box.Top - Metrics->Box.Bottom;
}

double pdfFontMetrics::Advance(char c) {
	if (!Characters) return 0;
	METRIC* Metrics = Characters[(unsigned char)c];
	if (!Metrics) return 0;
	return Metrics->Advance;
}

double pdfFontMetrics::Kerning(char c1, char c2) {
	if (!Characters) return 0.0;

	int c;

	METRIC* M1 = Characters[(unsigned char)c1];
	METRIC* M2 = Characters[(unsigned char)c2];
	KERNING* K;

	if (!M1) return 0.0;
	if (!M2) return 0.0;

	K = M1->Kerning;

	while (K) {
		c = Compare(M2->Name, K->Name);
		if (c < 0) {
			K = K->Left;
		}
		else if (c > 0) {
			K = K->Right;
		}
		else {
			return K->Offset;
		}
	}

	return 0.0;
}

void pdfFontMetrics::GetRect(
	const char* String,
	double* Left,
	double* Bottom,
	double* Right,
	double* Top
) {
	int j;
	for (j = 0; String[j]; j++);

	GetRect(String, j, Left, Bottom, Right, Top);
}

void pdfFontMetrics::GetRect(
	const char* String,
	unsigned    Length,
	double* Left,
	double* Bottom,
	double* Right,
	double* Top
) {
	if (!Characters || !Length) {
		*Left = *Bottom = *Right = *Top = 0.0;
		return;
	}

	double         l = 0.0;
	double         b = 0.0;
	double         r = 0.0;
	double         t = 0.0;
	double         a = 0.0;
	METRIC* Metrics;
	unsigned       j;
	unsigned char* s = (unsigned char*)String;

	for (j = 0; j < Length; j++) {
		Metrics = Characters[s[j]];
		if (Metrics) {
			l = Metrics->Box.Left;
			b = Metrics->Box.Bottom;
			t = Metrics->Box.Top;
			r = Metrics->Box.Right;
			a = Metrics->Advance;
			j++;
			break;
		}
	}

	for (; j < Length; j++) {
		Metrics = Characters[s[j]];
		if (Metrics) {
			if (b > Metrics->Box.Bottom) b = Metrics->Box.Bottom;
			if (t < Metrics->Box.Top) t = Metrics->Box.Top;
			r = a + Metrics->Box.Right + Kerning(s[j - 1], s[j]);
			a = a + Metrics->Advance;
		}
	}

	*Left = l;
	*Bottom = b;
	*Right = r;
	*Top = t;
}

void pdfFontMetrics::AddMetric(
	const char* Name,
	double      Left,
	double      Bottom,
	double      Right,
	double      Top,
	double      Advance
) {
	int j;

	METRIC* Temp = new METRIC;

	for (j = 0; Name[j]; j++);
	Temp->Name = new char[j + 1];

	for (j = 0; Name[j]; j++) {
		Temp->Name[j] = Name[j];
	}
	Temp->Name[j] = 0;

	Temp->Box.Left = Left;
	Temp->Box.Bottom = Bottom;
	Temp->Box.Right = Right;
	Temp->Box.Top = Top;
	Temp->Advance = Advance;

	if (!TheGlyphCount) { // This is the first glyph
		Box.Left = 1e12;
		Box.Bottom = 1e12;
		Box.Right = -1e12;
		Box.Top = -1e12;
	}

	if (Box.Left > Left) Box.Left = Left;
	if (Box.Bottom > Bottom) Box.Bottom = Bottom;
	if (Box.Right < Right) Box.Right = Right;
	if (Box.Top < Top) Box.Top = Top;

	if (!Insert(Temp)) delete Temp;
}
