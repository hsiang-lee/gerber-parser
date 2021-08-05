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

#ifndef PDFFONTMETRICS_H
#define PDFFONTMETRICS_H

#include "../afm/afmParser.h"
#include <stdio.h>

class pdfFontMetrics {
public:
	struct BOX {
		double Left;
		double Right;
		double Top;
		double Bottom;

		BOX();
	};

private:
	struct KERNING {
		char* Name;
		double Offset;

		bool     Red;
		KERNING* Left;
		KERNING* Right;
		KERNING* Parent;

		KERNING();
		~KERNING();
	};
	struct METRIC;
	void Restructure(METRIC* Container, KERNING* N);
	void FixDoubleRed(METRIC* Container, KERNING* N);
	bool Insert(METRIC* Container, KERNING* Node);

	struct METRIC {
		int      Code;
		char* Name;
		double   Advance; // The x-increment till the next character
		BOX      Box;
		KERNING* Kerning; // Red-Black tree (sorted by name)

		bool    Red;
		METRIC* Left;
		METRIC* Right;
		METRIC* Parent;

		METRIC();
		~METRIC();
	};
	void    Restructure(METRIC* N);
	void    FixDoubleRed(METRIC* N);
	bool    Insert(METRIC* Node);
	METRIC* FindMetric(const char* Name);

	METRIC* Metrics;    // Red-Black tree (sorted by name)
	METRIC** Characters; // Array; Index to the items in the tree

	afmParser  TheParser;
	afmParser* Parser;

	int  Compare(const char* s1, const char* s2);

	bool GetFontMetrics(); // The root of the parser

	bool GetCharMetrics();

	bool GetKernData();
	bool GetKernPairs();

public:
	pdfFontMetrics();
	~pdfFontMetrics();

	// Used by pdfType3Font to load the metrics, do not use directly
	bool LoadAFM(afmParser* Parser);

	// Loads the metrics from an AFM file
	bool LoadAFM(const char* FileName);

	// Changes the encoding to Windows ANSI
	// Call after loading the metrics
	void SetWinANSI();

	char* FontName;
	BOX   Box;

	void AddMetric(
		const char* Name,
		double      Left,
		double      Bottom,
		double      Right,
		double      Top,
		double      Advance
	);
	void AddKerning(const char* Name1, const char* Name2, double Offset);
	void SetEncoding(int Code, const char* Name);

	bool     HasGlyph(char c);
	unsigned GlyphCount();

	// Returns the code of the named character
	//  -1 => Glyph has no code
	//  -2 => Name not found
	int    Code(const   char* Name);
	void   Name(char c, char* Name);

	double Left(char c);
	double Bottom(char c);
	double Right(char c);
	double Top(char c);
	double Width(char c);
	double Height(char c);
	double Advance(char c);
	double Kerning(char c1, char c2);

	void GetRect(
		const char* String,
		double* Left,
		double* Bottom,
		double* Right,
		double* Top
	);
	void GetRect(
		const char* String,
		unsigned    Length,
		double* Left,
		double* Bottom,
		double* Right,
		double* Top
	);
};

#endif

