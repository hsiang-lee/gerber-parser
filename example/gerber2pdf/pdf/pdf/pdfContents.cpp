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

#include "pdfContents.h"
#include "pdfFont.h"
#include "pdfForm.h"
#include "pdfImage.h"


#define pi 3.141592653589793238463


pdfContents::pdfContents(bool UseCMYK) {
	this->UseCMYK = UseCMYK;

	CurrentFont = 0;
	FontSize = 0;

	X = Y = 0;
}


void pdfContents::Push() {
	AddLine("q");
}


void pdfContents::Pop() {
	AddLine("Q");
}


void pdfContents::Opaque(pdfOpaque* Opaque) {
	int l;
	char* Buffer;

	l = Opaque->Name.GetLength();
	Buffer = new char[l + 4];

	l = Opaque->Name.GetOutput(Buffer);

	Buffer[l++] = ' ';
	Buffer[l++] = 'g';
	Buffer[l++] = 's';
	Buffer[l++] = 0;

	AddLine(Buffer);

	delete[] Buffer;
}


void pdfContents::cm(
	double a, double b,
	double c, double d,
	double e, double f
) {
	int l;
	char* Buffer;
	pdfNumber na, nb, nc, nd, ne, nf;

	na = a;
	nb = b;
	nc = c;
	nd = d;
	ne = e;
	nf = f;

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
	Buffer[l++] = 'c';
	Buffer[l++] = 'm';
	Buffer[l++] = 0;

	AddLine(Buffer);

	delete[] Buffer;
}


void pdfContents::Use_mm() {
	Scale(72.0 / 25.4, 72.0 / 25.4);
}


void pdfContents::Translate(double x, double y) {
	cm(1, 0, 0, 1, x, y);
}


void pdfContents::Scale(double x, double y) {
	cm(x, 0, 0, y, 0, 0);
}


void pdfContents::Rotate(double angle) {
	angle *= pi / 180.0;
	cm(cos(angle), sin(angle), -sin(angle), cos(angle), 0, 0);
}


void pdfContents::Skew(double alpha, double beta) {
	alpha *= pi / 180.0;
	beta *= pi / 180.0;
	cm(1, tan(alpha), tan(beta), 1, 0, 0);
}


void pdfContents::LineWidth(double Width) {
	int l;
	pdfNumber n;
	char* Buffer;

	n = Width;

	l = n.GetLength();
	Buffer = new char[l + 3];

	n.GetOutput(Buffer);
	Buffer[l++] = ' ';
	Buffer[l++] = 'w';
	Buffer[l++] = 0;

	AddLine(Buffer);

	delete[] Buffer;
}


void pdfContents::LineCap(CapStyle Style) {
	int l;
	pdfNumber n;
	char* Buffer;

	n = Style;

	l = n.GetLength();
	Buffer = new char[l + 3];

	n.GetOutput(Buffer);
	Buffer[l++] = ' ';
	Buffer[l++] = 'J';
	Buffer[l++] = 0;

	AddLine(Buffer);

	delete[] Buffer;
}


void pdfContents::LineJoin(JoinStyle Style) {
	int l;
	pdfNumber n;
	char* Buffer;

	n = Style;

	l = n.GetLength();
	Buffer = new char[l + 3];

	n.GetOutput(Buffer);
	Buffer[l++] = ' ';
	Buffer[l++] = 'j';
	Buffer[l++] = 0;

	AddLine(Buffer);

	delete[] Buffer;
}


void pdfContents::LineStyle(double on, double off, double phase) {
	int l;
	pdfNumber a, b, c;
	char* Buffer;

	a = on;
	b = off;
	c = phase;

	l = 1 + a.GetLength() + 1 + b.GetLength() + 2 + c.GetLength();
	Buffer = new char[l + 3];

	l = 0;
	Buffer[l++] = '[';
	l += a.GetOutput(Buffer + l);
	Buffer[l++] = ' ';
	l += b.GetOutput(Buffer + l);
	Buffer[l++] = ']';
	Buffer[l++] = ' ';
	l += c.GetOutput(Buffer + l);

	Buffer[l++] = ' ';
	Buffer[l++] = 'd';
	Buffer[l++] = 0;

	AddLine(Buffer);

	delete[] Buffer;
}


void pdfContents::LineStyle(double* on, double* off, int count, double phase) {
	int        j, l;
	pdfNumber* n;
	char* Buffer;

	n = new pdfNumber[count * 2 + 1];

	l = 1;
	for (j = 0; j < count; j++) {
		n[j * 2] = on[j];
		l += n[j * 2].GetLength();
		l++;
		n[j * 2 + 1] = off[j];
		l += n[j * 2 + 1].GetLength();
		l++;
	}
	n[j * 2] = phase;
	l++;
	l += n[j * 2].GetLength();

	Buffer = new char[l + 3];

	l = 0;
	Buffer[l++] = '[';
	for (j = 0; j < count; j++) {
		l += n[j * 2].GetOutput(Buffer + l);
		Buffer[l++] = ' ';
		l += n[j * 2 + 1].GetOutput(Buffer + l);
		if (j < count - 1) Buffer[l++] = ' ';
	}
	Buffer[l++] = ']';
	Buffer[l++] = ' ';
	l += n[j * 2].GetOutput(Buffer + l);

	Buffer[l++] = ' ';
	Buffer[l++] = 'd';
	Buffer[l++] = 0;

	AddLine(Buffer);

	delete[] Buffer;
	delete[] n;
}


// https://www.101computing.net/cmyk-to-rgb-conversion-algorithm/
void pdfContents::RGB_to_CMYK(
	double  red, double  green, double  blue,
	double& cyan, double& magenta, double& yellow, double& black
) {
	double          max = red;
	if (max < green) max = green;
	if (max < blue) max = blue;

	black = 1.0 - max;

	if (black == 1.0) {
		cyan = 0.0;
		magenta = 0.0;
		yellow = 0.0;
	}
	else {
		cyan = (1.0 - red - black) / (1.0 - black);
		magenta = (1.0 - green - black) / (1.0 - black);
		yellow = (1.0 - blue - black) / (1.0 - black);
	}
}


// https://www.101computing.net/cmyk-to-rgb-conversion-algorithm/
void pdfContents::CMYK_to_RGB(
	double  cyan, double  magenta, double  yellow, double  black,
	double& red, double& green, double& blue
) {
	if (black == 1.0) {
		red = 0.0;
		green = 0.0;
		blue = 0.0;
	}
	else {
		red = (1.0 - cyan) * (1.0 - black);
		green = (1.0 - magenta) * (1.0 - black);
		blue = (1.0 - yellow) * (1.0 - black);
	}
}


void pdfContents::StrokeColour(double red, double green, double blue) {
	if (UseCMYK) {
		double c, m, y, k;
		RGB_to_CMYK(red, green, blue, c, m, y, k);
		StrokeColour(c, m, y, k);
		return;
	}

	int l;
	char* Buffer;
	pdfNumber r, g, b;

	r = red;
	g = green;
	b = blue;

	l = r.GetLength() + 1 + g.GetLength() + 1 + b.GetLength();
	Buffer = new char[l + 4];

	l = r.GetOutput(Buffer);
	Buffer[l++] = ' ';
	l += g.GetOutput(Buffer + l);
	Buffer[l++] = ' ';
	l += b.GetOutput(Buffer + l);

	Buffer[l++] = ' ';
	Buffer[l++] = 'R';
	Buffer[l++] = 'G';
	Buffer[l++] = 0;

	AddLine(Buffer);

	delete[] Buffer;
}


void pdfContents::StrokeColour(double cyan, double magenta, double yellow, double black) {
	if (!UseCMYK) {
		double r, g, b;
		CMYK_to_RGB(cyan, magenta, yellow, black, r, g, b);
		StrokeColour(r, g, b);
		return;
	}

	int l;
	char* Buffer;
	pdfNumber c, m, y, k;

	c = cyan;
	m = magenta;
	y = yellow;
	k = black;

	l = c.GetLength() + 1 + m.GetLength() + 1 + y.GetLength() + 1 + k.GetLength();
	Buffer = new char[l + 3];

	l = c.GetOutput(Buffer);
	Buffer[l++] = ' ';
	l += m.GetOutput(Buffer + l);
	Buffer[l++] = ' ';
	l += y.GetOutput(Buffer + l);
	Buffer[l++] = ' ';
	l += k.GetOutput(Buffer + l);

	Buffer[l++] = ' ';
	Buffer[l++] = 'K';
	Buffer[l++] = 0;

	AddLine(Buffer);

	delete[] Buffer;
}


void pdfContents::FillColour(double red, double green, double blue) {
	if (UseCMYK) {
		double c, m, y, k;
		RGB_to_CMYK(red, green, blue, c, m, y, k);
		FillColour(c, m, y, k);
		return;
	}

	int l;
	char* Buffer;
	pdfNumber r, g, b;

	r = red;
	g = green;
	b = blue;

	l = r.GetLength() + 1 + g.GetLength() + 1 + b.GetLength();
	Buffer = new char[l + 4];

	l = r.GetOutput(Buffer);
	Buffer[l++] = ' ';
	l += g.GetOutput(Buffer + l);
	Buffer[l++] = ' ';
	l += b.GetOutput(Buffer + l);

	Buffer[l++] = ' ';
	Buffer[l++] = 'r';
	Buffer[l++] = 'g';
	Buffer[l++] = 0;

	AddLine(Buffer);

	delete[] Buffer;
}


void pdfContents::FillColour(double cyan, double magenta, double yellow, double black) {
	if (!UseCMYK) {
		double r, g, b;
		CMYK_to_RGB(cyan, magenta, yellow, black, r, g, b);
		FillColour(r, g, b);
		return;
	}

	int l;
	char* Buffer;
	pdfNumber c, m, y, k;

	c = cyan;
	m = magenta;
	y = yellow;
	k = black;

	l = c.GetLength() + 1 + m.GetLength() + 1 + y.GetLength() + 1 + k.GetLength();
	Buffer = new char[l + 3];

	l = c.GetOutput(Buffer);
	Buffer[l++] = ' ';
	l += m.GetOutput(Buffer + l);
	Buffer[l++] = ' ';
	l += y.GetOutput(Buffer + l);
	Buffer[l++] = ' ';
	l += k.GetOutput(Buffer + l);

	Buffer[l++] = ' ';
	Buffer[l++] = 'k';
	Buffer[l++] = 0;

	AddLine(Buffer);

	delete[] Buffer;
}


// Finds the bezier control points for a best fit to an arc centered at (0, 0)
void FindControlPoints(
	double  x1, double  y1, // Start point
	double  a,             // Angle
	double* x2, double* y2, // Control point 1
	double* x3, double* y3, // Control point 2
	double* x4, double* y4  // End point
) {
	double r;      // Radius
	double x5, y5; // Midpoint on arc
	double x6, y6; // Intersection of tangents
	double a1, a4, a5;
	double b, t, t1, t2;
	double e, de, dt, rb, xb, yb;

	a *= pi / 180.0;
	b = a / 2.0;

	r = sqrt(y1 * y1 + x1 * x1);
	a1 = atan2(y1, x1);

	a4 = a1 + a;
	*x4 = r * cos(a4);
	*y4 = r * sin(a4);

	a5 = a1 + b;
	x5 = r * cos(a5);
	y5 = r * sin(a5);

	b = cos(b);
	x6 = x5 / b;
	y6 = y5 / b;

	// Best extimate
	t1 = 6 * x6 - 3 * x1 - 3 * (*x4);
	t2 = 6 * y6 - 3 * y1 - 3 * (*y4);
	if (fabs(t1) > fabs(t2)) {
		t = (8 * x5 - 4 * x1 - 4 * (*x4)) / t1;
	}
	else {
		t = (8 * y5 - 4 * y1 - 4 * (*y4)) / t2;
	}

	// Newton-Raphson to fit B(1/3) to the circle
	r = r * r;

	dt = 1e-3;
	t2 = 1.0 / 3.0; // Fitting B(1/3) to the circle is close enough to the
	t1 = 1.0 - t2;  // optimum solution.

	*x2 = (1.0 - t) * x1 + t * x6;
	*y2 = (1.0 - t) * y1 + t * y6;
	*x3 = (1.0 - t) * (*x4) + t * x6;
	*y3 = (1.0 - t) * (*y4) + t * y6;
	xb = t1 * t1 * t1 * x1 + 3.0 * t1 * t1 * t2 * (*x2) + 3.0 * t1 * t2 * t2 * (*x3) + t2 * t2 * t2 * (*x4);
	yb = t1 * t1 * t1 * y1 + 3.0 * t1 * t1 * t2 * (*y2) + 3.0 * t1 * t2 * t2 * (*y3) + t2 * t2 * t2 * (*y4);
	rb = xb * xb + yb * yb;
	e = rb - r;
	while (e > 1e-12) {
		*x2 = (1.0 - (t + dt)) * x1 + (t + dt) * x6;
		*y2 = (1.0 - (t + dt)) * y1 + (t + dt) * y6;
		*x3 = (1.0 - (t + dt)) * (*x4) + (t + dt) * x6;
		*y3 = (1.0 - (t + dt)) * (*y4) + (t + dt) * y6;
		xb = t1 * t1 * t1 * x1 + 3.0 * t1 * t1 * t2 * (*x2) + 3.0 * t1 * t2 * t2 * (*x3) + t2 * t2 * t2 * (*x4);
		yb = t1 * t1 * t1 * y1 + 3.0 * t1 * t1 * t2 * (*y2) + 3.0 * t1 * t2 * t2 * (*y3) + t2 * t2 * t2 * (*y4);
		rb = xb * xb + yb * yb;
		de = (rb - r - e) / dt;

		t -= e / de; // Newton-Raphson

		*x2 = (1.0 - t) * x1 + t * x6;
		*y2 = (1.0 - t) * y1 + t * y6;
		*x3 = (1.0 - t) * (*x4) + t * x6;
		*y3 = (1.0 - t) * (*y4) + t * y6;
		xb = t1 * t1 * t1 * x1 + 3.0 * t1 * t1 * t2 * (*x2) + 3.0 * t1 * t2 * t2 * (*x3) + t2 * t2 * t2 * (*x4);
		yb = t1 * t1 * t1 * y1 + 3.0 * t1 * t1 * t2 * (*y2) + 3.0 * t1 * t2 * t2 * (*y3) + t2 * t2 * t2 * (*y4);
		rb = xb * xb + yb * yb;
		e = rb - r;
	}
}


void pdfContents::Rectangle(double x, double y, double w, double h) {
	int l;
	pdfNumber a, b, c, d;
	char* Buffer;

	a = x;
	b = y;
	c = w;
	d = h;

	l = a.GetLength() + 1 +
		b.GetLength() + 1 +
		c.GetLength() + 1 +
		d.GetLength();
	Buffer = new char[l + 4];

	l = a.GetOutput(Buffer);
	Buffer[l++] = ' ';
	l += b.GetOutput(Buffer + l);
	Buffer[l++] = ' ';
	l += c.GetOutput(Buffer + l);
	Buffer[l++] = ' ';
	l += d.GetOutput(Buffer + l);

	Buffer[l++] = ' ';
	Buffer[l++] = 'r';
	Buffer[l++] = 'e';
	Buffer[l++] = 0;

	AddLine(Buffer);

	X = x;
	Y = y;

	delete[] Buffer;
}


void pdfContents::Circle(double x, double y, double r) {
	BeginLine(x + r, y);
	Arc(x, y, 360);
}


void pdfContents::Ellipse(double x, double y, double rx, double ry) {
	double x1, y1; // Bezier Control 1
	double x2, y2; // Bezier Control 2
	double x3, y3; // End

	BeginLine(x + rx, y);

	FindControlPoints(X - x, Y - y, 90, &x1, &y1, &x2, &y2, &x3, &y3);
	Bezier(x + x1, y + y1 / rx * ry, x + x2, y + y2 / rx * ry, x + x3, y + y3 / rx * ry);
	Y = y + y3;

	FindControlPoints(X - x, Y - y, 90, &x1, &y1, &x2, &y2, &x3, &y3);
	Bezier(x + x1, y + y1 / rx * ry, x + x2, y + y2 / rx * ry, x + x3, y + y3 / rx * ry);

	FindControlPoints(X - x, Y - y, 90, &x1, &y1, &x2, &y2, &x3, &y3);
	Bezier(x + x1, y + y1 / rx * ry, x + x2, y + y2 / rx * ry, x + x3, y + y3 / rx * ry);
	Y = y + y3;

	FindControlPoints(X - x, Y - y, 90, &x1, &y1, &x2, &y2, &x3, &y3);
	Bezier(x + x1, y + y1 / rx * ry, x + x2, y + y2 / rx * ry, x + x3, y + y3 / rx * ry);
}


void pdfContents::BeginLine(double x, double y) {
	int l;
	pdfNumber a, b;
	char* Buffer;

	a = x;
	b = y;

	l = a.GetLength() + 1 + b.GetLength();
	Buffer = new char[l + 3];

	l = a.GetOutput(Buffer);
	Buffer[l++] = ' ';
	l += b.GetOutput(Buffer + l);

	Buffer[l++] = ' ';
	Buffer[l++] = 'm';
	Buffer[l++] = 0;

	AddLine(Buffer);

	X = x;
	Y = y;

	delete[] Buffer;
}


void pdfContents::Line(double x, double y) {
	int l;
	pdfNumber a, b;
	char* Buffer;

	a = x;
	b = y;

	l = a.GetLength() + 1 + b.GetLength();
	Buffer = new char[l + 3];

	l = a.GetOutput(Buffer);
	Buffer[l++] = ' ';
	l += b.GetOutput(Buffer + l);

	Buffer[l++] = ' ';
	Buffer[l++] = 'l';
	Buffer[l++] = 0;

	AddLine(Buffer);

	X = x;
	Y = y;

	delete[] Buffer;
}


void pdfContents::Arc(double x, double y, double a) {
	double x1, y1; // Bezier Control 1
	double x2, y2; // Bezier Control 2
	double x3, y3; // End

	if (fabs(X - x) < 1e-15 && fabs(Y - y) < 1e-15) return;

	if (fabs(a) > 45.001) { // Slightly large so that the circle has 4 segments
		a /= 2.0;
		Arc(x, y, a);
		Arc(x, y, a);
		return;
	}

	FindControlPoints(X - x, Y - y, a, &x1, &y1, &x2, &y2, &x3, &y3);

	Bezier(x + x1, y + y1, x + x2, y + y2, x + x3, y + y3);
}


void pdfContents::Bezier(
	double c1x, double c1y,
	double c2x, double c2y,
	double   x, double   y
) {
	int l;
	pdfNumber a, b, c, d, e, f;
	char* Buffer;

	a = c1x;
	b = c1y;
	c = c2x;
	d = c2y;
	e = x;
	f = y;

	l = a.GetLength() + 1 +
		b.GetLength() + 1 +
		c.GetLength() + 1 +
		d.GetLength() + 1 +
		e.GetLength() + 1 +
		f.GetLength();
	Buffer = new char[l + 3];

	l = a.GetOutput(Buffer);
	Buffer[l++] = ' ';
	l += b.GetOutput(Buffer + l);
	Buffer[l++] = ' ';
	l += c.GetOutput(Buffer + l);
	Buffer[l++] = ' ';
	l += d.GetOutput(Buffer + l);
	Buffer[l++] = ' ';
	l += e.GetOutput(Buffer + l);
	Buffer[l++] = ' ';
	l += f.GetOutput(Buffer + l);

	Buffer[l++] = ' ';
	Buffer[l++] = 'c';
	Buffer[l++] = 0;

	AddLine(Buffer);

	X = x;
	Y = y;

	delete[] Buffer;
}


void pdfContents::Close() {
	AddLine("h");
}


void pdfContents::Stroke() {
	AddLine("S");
}


void pdfContents::Fill() {
	AddLine("f");
}


void pdfContents::FillEvenOdd() {
	AddLine("f*");
}


void pdfContents::FillStroke() {
	AddLine("B");
}


void pdfContents::Clip() {
	AddLine("W* n");
}


void pdfContents::BeginText() {
	AddLine("BT");
}


void pdfContents::Font(pdfFont* Font, double Size) {
	int   l;
	char* Buffer;

	pdfNumber size;

	CurrentFont = Font;
	FontSize = Size;

	size = Size;

	l = Font->Name.GetLength() + 1 + size.GetLength();
	Buffer = new char[l + 4];

	l = Font->Name.GetOutput(Buffer);
	Buffer[l++] = ' ';
	l += size.GetOutput(Buffer + l);

	Buffer[l++] = ' ';
	Buffer[l++] = 'T';
	Buffer[l++] = 'f';
	Buffer[l++] = 0;

	AddLine(Buffer);

	delete[] Buffer;
}


void pdfContents::Raise(double Raise) {
	int   l;
	char* Buffer;

	pdfNumber raise;

	raise = Raise;

	l = raise.GetLength();
	Buffer = new char[l + 4];

	l = raise.GetOutput(Buffer);

	Buffer[l++] = ' ';
	Buffer[l++] = 'T';
	Buffer[l++] = 's';
	Buffer[l++] = 0;

	AddLine(Buffer);

	delete[] Buffer;
}


void pdfContents::TextPos_mm(double x, double y) {
	TextPos(x / 25.4 * 72, y / 25.4 * 72);
}


void pdfContents::TextPos(double x, double y) {
	int   l;
	char* Buffer;

	pdfNumber a, b;

	a = x;
	b = y;

	l = a.GetLength() + 1 + b.GetLength();
	Buffer = new char[l + 4];

	l = a.GetOutput(Buffer);
	Buffer[l++] = ' ';
	l += b.GetOutput(Buffer + l);

	Buffer[l++] = ' ';
	Buffer[l++] = 'T';
	Buffer[l++] = 'd';
	Buffer[l++] = 0;

	AddLine(Buffer);

	delete[] Buffer;
}


void pdfContents::LineSpacing(double Spacing) {
	int   l;
	char* Buffer;

	pdfNumber spacing;

	spacing = Spacing;

	l = spacing.GetLength();
	Buffer = new char[l + 4];

	l = spacing.GetOutput(Buffer);

	Buffer[l++] = ' ';
	Buffer[l++] = 'T';
	Buffer[l++] = 'L';
	Buffer[l++] = 0;

	AddLine(Buffer);

	delete[] Buffer;
}


void pdfContents::Text(const char* Text) {
	int j;
	for (j = 0; Text[j]; j++);
	pdfContents::Text(Text, j);
}


void pdfContents::Text(const char* Text, int Length) {
	int    j, q, l;
	bool   Kerning = false;
	char* Buffer;
	double Offset;

	pdfString String;
	pdfNumber Number;

	for (j = 1; j < Length; j++) {
		if (CurrentFont->Metrics.Kerning(Text[j - 1], Text[j]) != 0.0) {
			Kerning = true;
			break;
		}
	}

	String.Set(Text, Length);

	l = String.GetLength();
	Buffer = new char[l * 10];

	if (Kerning) {
		l = 0;
		Buffer[l++] = '[';
		j = 0;
		for (q = 1; q < Length; q++) {
			Offset = CurrentFont->Metrics.Kerning(Text[q - 1], Text[q]);
			if (Offset != 0.0) {
				String.Set(Text + j, q - j);
				Number = -Offset;
				l += String.GetOutput(Buffer + l);
				l += Number.GetOutput(Buffer + l);
				j = q;
			}
		}
		String.Set(Text + j, q - j);
		l += String.GetOutput(Buffer + l);
		Buffer[l++] = ']';
		Buffer[l++] = ' ';
		Buffer[l++] = 'T';
		Buffer[l++] = 'J';
		Buffer[l++] = 0;

	}
	else { // No kerning
		l = String.GetOutput(Buffer);
		Buffer[l++] = ' ';
		Buffer[l++] = 'T';
		Buffer[l++] = 'j';
		Buffer[l++] = 0;
	}

	AddLine(Buffer);

	delete[] Buffer;
}


void pdfContents::Subscript(const char* Text) {
	if (!CurrentFont) return;

	int PrevSize = FontSize;

	Font(CurrentFont, FontSize * 0.7);
	Raise(-1.5 / 7.0 * FontSize);
	pdfContents::Text(Text);
	Raise(0);

	Font(CurrentFont, PrevSize);
}


void pdfContents::Superscript(const char* Text) {
	if (!CurrentFont) return;

	int PrevSize = FontSize;

	Font(CurrentFont, FontSize * 0.7);
	Raise(0.52 * FontSize);
	pdfContents::Text(Text);
	Raise(0);

	Font(CurrentFont, PrevSize);
}


void pdfContents::NewLine() {
	AddLine("T*");
}


void pdfContents::EndText() {
	AddLine("ET");
}


void pdfContents::Form(pdfForm* Form) {
	int l;
	char* Buffer;

	l = Form->Name.GetLength();
	Buffer = new char[l + 4];

	Form->Name.GetOutput(Buffer);
	Buffer[l++] = ' ';
	Buffer[l++] = 'D';
	Buffer[l++] = 'o';
	Buffer[l++] = 0;

	AddLine(Buffer);

	delete[] Buffer;
}


void pdfContents::Image(pdfImage* Image, double Scale) {
	int l;
	char* Buffer;

	Push();
	pdfContents::Scale(Image->Width_mm * Scale, Image->Height_mm * Scale);

	l = Image->Name.GetLength();
	Buffer = new char[l + 4];

	Image->Name.GetOutput(Buffer);
	Buffer[l++] = ' ';
	Buffer[l++] = 'D';
	Buffer[l++] = 'o';
	Buffer[l++] = 0;

	AddLine(Buffer);
	Pop();

	delete[] Buffer;
}

