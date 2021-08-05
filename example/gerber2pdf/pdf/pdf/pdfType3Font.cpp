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

#include "pdfType3Font.h"

pdfType3Font::pdfType3Font(const char* Name, bool UseCMYK) : pdfFont(Name) {
	this->UseCMYK = UseCMYK;

	Widths.Object = &WidthsArray;
	Encoding.Object = &EncodingDictionary;
	CharProcs.Object = &CharProcsDictionary;

	Left = 0;
	Bottom = 0;
	Right = 0;
	Top = 0;
	FontBBox.Add(&Left);
	FontBBox.Add(&Bottom);
	FontBBox.Add(&Right);
	FontBBox.Add(&Top);

	Thousandth = 0.001;
	Zero = 0.000;
	FontMatrix.Add(&Thousandth);
	FontMatrix.Add(&Zero);
	FontMatrix.Add(&Zero);
	FontMatrix.Add(&Thousandth);
	FontMatrix.Add(&Zero);
	FontMatrix.Add(&Zero);

	Subtype.Set("Type3");

	Resources.OneLine = true;

	EncodingType.Set("Encoding");
	EncodingDictionary.AddEntry("Type", &EncodingType);
	EncodingDictionary.AddEntry("Differences", &Differences);

	TheWidths = 0;
	DifferencesCode = 0;
	DifferencesName = 0;

	Update();
}

pdfType3Font::~pdfType3Font() {
	if (TheWidths) delete[] TheWidths;
	if (DifferencesCode) delete[] DifferencesCode;
	if (DifferencesName) delete[] DifferencesName;
}

void pdfType3Font::Update() {
	Dictionary.Clear();

	Dictionary.AddEntry("Type", &Type);
	Dictionary.AddEntry("Subtype", &Subtype);
	Dictionary.AddEntry("Widths", &Widths);
	Dictionary.AddEntry("FontBBox", &FontBBox);
	Dictionary.AddEntry("Encoding", &Encoding);
	Dictionary.AddEntry("LastChar", &LastChar);
	Dictionary.AddEntry("CharProcs", &CharProcs);
	Dictionary.AddEntry("FirstChar", &FirstChar);
	if (Resources.GetCount()) {
		Dictionary.AddEntry("Resources", &Resources);
	}
	Dictionary.AddEntry("FontMatrix", &FontMatrix);
}

void pdfType3Font::SetWidths() {
	int j, first, last;

	for (first = 0; first < 0x100 && !Metrics.HasGlyph(first); first++);
	for (last = 0xFF; last > 0 && !Metrics.HasGlyph(last); last--);

	if (first > last) first = last;
	FirstChar = first;
	LastChar = last;

	if (TheWidths) delete[] TheWidths;
	TheWidths = new pdfNumber[last - first + 1];

	WidthsArray.Clear();
	for (j = 0; j <= last - first; j++) {
		TheWidths[j] = Metrics.Advance(first + j);
		WidthsArray.Add(TheWidths + j);
	}
}

void pdfType3Font::SetEncoding() {
	int  j, q, N, first;
	char String[0x100]; // Assume a name is never longer than 0x100 characters

	Differences.Clear();
	if (DifferencesCode) delete[] DifferencesCode;
	if (DifferencesName) delete[] DifferencesName;

	first = FirstChar.Value;

	N = LastChar.Value - first + 1;

	// Wastes memory, but little enough
	DifferencesCode = new pdfNumber[N];
	DifferencesName = new pdfName[N];

	q = 0;
	DifferencesCode[q] = first;
	Differences.Add(DifferencesCode);
	q++;
	for (j = 0; j < N; j++) {
		if (Metrics.HasGlyph(first + j)) {
			Metrics.Name(first + j, String);
			DifferencesName[j].Set(String);
			Differences.Add(DifferencesName + j);

		}
		else {
			j++;
			while (j < N && !Metrics.HasGlyph(first + j)) j++;
			if (j < N) {
				DifferencesCode[q] = first + j;
				Differences.Add(DifferencesCode + q);
				q++;
				j--; // So the loop can add one and still be correct
			}
		}
	}
}

pdfContents* pdfType3Font::NewGlyph(
	const char* Name,
	double      Left,
	double      Bottom,
	double      Right,
	double      Top,
	double      Advance
) {
	int       l;
	char* Buffer;
	pdfNumber na, nb, nc, nd, ne, nf;

	pdfContents* Contents = new pdfContents(UseCMYK);

	Metrics.AddMetric(Name, Left, Bottom, Right, Top, Advance);

	pdfType3Font::Left = Metrics.Box.Left;
	pdfType3Font::Bottom = Metrics.Box.Bottom;
	pdfType3Font::Right = Metrics.Box.Right;
	pdfType3Font::Top = Metrics.Box.Top;

	CharProcsDictionary.AddEntry(Name, Contents);

	// Add "w_x w_y ll_x ll_y ur_x ur_y d1"
	na = Advance;
	nb = 0;
	nc = Left;
	nd = Bottom;
	ne = Right;
	nf = Top;

	l = na.GetLength() + 1 +
		nb.GetLength() + 1 +
		nc.GetLength() + 1 +
		nd.GetLength() + 1 +
		ne.GetLength() + 1 +
		nf.GetLength();
	Buffer = new char[l + 4];

	l = na.GetOutput(Buffer);
	Buffer[l++] = ' ';
	l += nb.GetOutput(Buffer + l);
	Buffer[l++] = ' ';
	l += nc.GetOutput(Buffer + l);
	Buffer[l++] = ' ';
	l += nd.GetOutput(Buffer + l);
	Buffer[l++] = ' ';
	l += ne.GetOutput(Buffer + l);
	Buffer[l++] = ' ';
	l += nf.GetOutput(Buffer + l);

	Buffer[l++] = ' ';
	Buffer[l++] = 'd';
	Buffer[l++] = '1';
	Buffer[l++] = 0;

	Contents->AddLine(Buffer);

	delete[] Buffer;

	return Contents;
}

int pdfType3Font::Compare(const char* s1, const char* s2) {
	int j;

	for (j = 0; s1[j] && s2[j]; j++) {
		if (s1[j] < s2[j]) return -1;
		if (s1[j] > s2[j]) return  1;
	}

	if (s1[j]) return  1;
	if (s2[j]) return -1;

	return 0;
}

pdfContents* pdfType3Font::GetGlyph() {
	char    Key[0x100];
	char    Name[0x100];
	double  d, x, y;
	double  c1x, c1y, c2x, c2y;
	double  w, h, r, rx, ry, a;

	Parser.GetName(Name);
	Parser.GetEndOfLine();

	pdfContents* Glyph = NewGlyph(
		Name,
		Metrics.Left(Metrics.Code(Name)),
		Metrics.Bottom(Metrics.Code(Name)),
		Metrics.Right(Metrics.Code(Name)),
		Metrics.Top(Metrics.Code(Name)),
		Metrics.Advance(Metrics.Code(Name))
	);

	while (Parser.GetKey(Key)) {
		if (!Compare(Key, "EndGlyph")) {
			Glyph->Deflate();
			return Glyph;

		}
		else if (!Compare(Key, "CS")) { // Cap Style
			Parser.GetNumber(&d);
			Glyph->LineCap((pdfContents::CapStyle)((int)d));

		}
		else if (!Compare(Key, "JS")) { // Join Style
			Parser.GetNumber(&d);
			Glyph->LineJoin((pdfContents::JoinStyle)((int)d));

		}
		else if (!Compare(Key, "LW")) { // Line width
			Parser.GetNumber(&d);
			Glyph->LineWidth(d);

		}
		else if (!Compare(Key, "Rect")) { // Rectangle
			Parser.GetNumber(&x);
			Parser.GetNumber(&y);
			Parser.GetNumber(&w);
			Parser.GetNumber(&h);
			Glyph->Rectangle(x, y, w, h);

		}
		else if (!Compare(Key, "Circ")) { // Circle
			Parser.GetNumber(&x);
			Parser.GetNumber(&y);
			Parser.GetNumber(&r);
			Glyph->Circle(x, y, r);

		}
		else if (!Compare(Key, "Ellipse")) { // Ellipse
			Parser.GetNumber(&x);
			Parser.GetNumber(&y);
			Parser.GetNumber(&rx);
			Parser.GetNumber(&ry);
			Glyph->Ellipse(x, y, rx, ry);

		}
		else if (!Compare(Key, "BL")) { // Begin Line
			Parser.GetNumber(&x);
			Parser.GetNumber(&y);
			Glyph->BeginLine(x, y);

		}
		else if (!Compare(Key, "L")) { // Line
			Parser.GetNumber(&x);
			Parser.GetNumber(&y);
			Glyph->Line(x, y);

		}
		else if (!Compare(Key, "A")) { // Arc
			Parser.GetNumber(&x);
			Parser.GetNumber(&y);
			Parser.GetNumber(&a);
			Glyph->Arc(x, y, a);

		}
		else if (!Compare(Key, "B")) { // Cubic Bezier
			Parser.GetNumber(&c1x);
			Parser.GetNumber(&c1y);
			Parser.GetNumber(&c2x);
			Parser.GetNumber(&c2y);
			Parser.GetNumber(&x);
			Parser.GetNumber(&y);
			Glyph->Bezier(c1x, c1y, c2x, c2y, x, y);

		}
		else if (!Compare(Key, "C")) { // Close
			Glyph->Close();

		}
		else if (!Compare(Key, "S")) { // Stroke
			Glyph->Stroke();

		}
		else if (!Compare(Key, "F")) { // Fill
			Glyph->Fill();

		}
		else if (!Compare(Key, "FS")) { // Fill and Stroke
			Glyph->FillStroke();

		}
		else { // Ignore others
			printf("Warning: Unknown path operator %s\n", Key);
			if (!Parser.Ignore(Key)) return 0;
		}
	}

	return 0;
}

pdfContents** pdfType3Font::GetGlyphs(unsigned* GlyphCount) {
	int  Count;
	char Key[0x100];

	*GlyphCount = 0;

	Parser.GetWhiteSpace();
	Parser.GetInteger(&Count);
	Parser.GetEndOfLine();

	pdfContents** Glyphs = new pdfContents * [Count];

	while (Parser.GetKey(Key)) {
		if (!Compare(Key, "EndFontGlyphs")) {
			if ((int)(*GlyphCount) < Count - 1) printf("Error: Too few glyph declarations");
			return Glyphs;

		}
		else if (!Compare(Key, "StartGlyph")) {
			if ((int)(*GlyphCount) < Count) {
				Glyphs[(*GlyphCount)++] = GetGlyph();
			}
			else {
				printf("Error: Too many glyph declarations\n");
				Parser.GetEndOfLine();
			}

		}
		else { // Ignore others
			if (!Parser.Ignore(Key)) return 0;
		}
	}

	printf("Error: \"EndFontGlyphs\" expected\n");
	*GlyphCount = 0;
	return 0;
}

pdfContents** pdfType3Font::GetFontGlyphs(unsigned* GlyphCount) {
	char String[0x1000];

	while (Parser.GetKey(String)) {
		if (!Compare(String, "StartFontGlyphs")) {
			return GetGlyphs(GlyphCount);

		}
		else { // Ignore others
			printf("Warning: No glyphs to load\n");
			if (!Parser.Ignore(String)) return 0;
		}
	}

	return 0;
}

pdfContents** pdfType3Font::LoadT3F(const char* FileName, unsigned* GlyphCount) {
	pdfContents** Glyphs;

	*GlyphCount = 0;

	if (Parser.Open(FileName)) {
		if (!Metrics.LoadAFM(&Parser)) return 0;
		Parser.GetEndOfLine();
		Glyphs = GetFontGlyphs(GlyphCount);
		Parser.Close();
		return Glyphs;
	}

	return 0;
}
