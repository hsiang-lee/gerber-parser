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

#ifndef PDFCONTENTS_H
#define PDFCONTENTS_H


#include <math.h>


#include "pdfStream.h"
#include "pdfArray.h"
#include "pdfNumber.h"
#include "pdfString.h"
#include "pdfOpaque.h"


class pdfFont;
class pdfForm;
class pdfImage;


class pdfContents : public pdfStream {
public:
	enum CapStyle {
		csButt = 0,
		csRound = 1,
		csSquare = 2
	};

	enum JoinStyle {
		jsMiter = 0,
		jsRound = 1,
		jsBevel = 2
	};

private:
	bool     UseCMYK;
	pdfFont* CurrentFont;
	double   FontSize; // The current font size
	double   X, Y;     // The current point when drawing lines

	void cm(double a, double b, double c, double d, double e, double f);

	void RGB_to_CMYK(double  red, double  green, double  blue,
		double& cyan, double& magenta, double& yellow, double& black);
	void CMYK_to_RGB(double  cyan, double  magenta, double  yellow, double  black,
		double& red, double& green, double& blue);

public:
	pdfContents(bool UseCMYK = false);

	void Push();
	void Pop();

	void Use_mm(); // Transforms the space such that all dimentions are in mm

	void Translate(double x, double y);
	void Scale(double x, double y);
	void Rotate(double angle);
	void Skew(double alpha, double beta);

	void LineWidth(double    Width);
	void LineCap(CapStyle  Style);
	void LineJoin(JoinStyle Style);
	void LineStyle(double  on, double  off, double phase);
	void LineStyle(double* on, double* off, int count, double phase);

	void StrokeColour(double red, double green, double blue);                 // [0.0, 1.0]
	void StrokeColour(double cyan, double magenta, double yellow, double black); // [0.0, 1.0]
	void FillColour(double red, double green, double blue);                 // [0.0, 1.0]
	void FillColour(double cyan, double magenta, double yellow, double black); // [0.0, 1.0]
	void Opaque(pdfOpaque* Opaque);

	// Use these to begin a line
	void Rectangle(double x, double y, double w, double h);
	void Circle(double x, double y, double r); // < 4 ppm error (8x bezier)
	void Ellipse(double x, double y, double rx, double ry);
	void BeginLine(double x, double y);

	// Use these to add line segments
	void Line(double   x, double   y);
	void Arc(double   x, double   y,  // Center; < 4 ppm error
		double   a);             // Angle [deg], positive => CCW
	void Bezier(double c1x, double c1y,  // Control point 1
		double c2x, double c2y,  // Control point 2
		double   x, double   y); // Final   point
	void Close();

	// Use these to draw the line
	void Stroke();
	void Fill();
	void FillEvenOdd();
	void FillStroke();
	void Clip(); // Add to clipping path

	// Use these to draw text
	void BeginText();
	void TextPos(double x, double y);
	void TextPos_mm(double x, double y);
	void LineSpacing(double Spacing);
	void Font(pdfFont* Font, double Size);
	void Raise(double Raise);
	void Text(const char* Text); // null-terminated
	void Text(const char* Text, int Length);
	void Subscript(const char* Text);
	void Superscript(const char* Text);
	void NewLine();
	void EndText();

	// Use these to draw XObjects
	void Form(pdfForm* Form);
	void Image(pdfImage* Image, double Scale = 1.0);
};


#endif


