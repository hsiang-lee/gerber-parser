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

#include "pdfNumber.h"


pdfNumber::pdfNumber() {
	Value = 0.0;
}


double pdfNumber::operator= (double Number) {
	Value = Number;
	return Number;
}


static int mod10(double f) {
	if (f < 10e16) {
		return round(f - 10.0 * floor(f / 10.0));
	}
	else {
		return 0;
	}
}


int pdfNumber::GetLength() {
	int    r = 0;
	int    d;
	double scale;
	double i, f; // integer and fraction
	double val;

	if (Value == NAN ||
		Value == INFINITY ||
		Value == -INFINITY) return 1;

	val = Value;
	if (val < 0.0) {
		r++;
		val *= -1.0;
	}

	i = val;
	d = 10;
	scale = 10e9;
	while (i > 1.0) {
		scale /= 10.0;
		i /= 10.0;
		d--;
	}

	i = floor(val);
	f = round((val - i) * scale);
	while (f >= scale) {
		i++;
		f = round(f - scale);
	}

	if (i == 0.0) r++; // The leading zero
	while (i >= 1.0) {
		r++;
		i = floor(i / 10.0);
	}

	IntegerPart = r;

	bool b = false;
	if (f > 0.0) {
		r++; // The decimal point
		while (d) {
			if (b || (mod10(f) > 0)) {
				r++;
				b = true;
			}
			f = floor(f / 10.0);
			d--;
		}
	}

	return r;
}


int pdfNumber::GetOutput(char* Buffer) {
	int    l;
	int    d;
	double scale;
	double val;
	double i, f; // integer and fraction
	int    j;     // The index;

	if (Value == NAN ||
		Value == INFINITY ||
		Value == -INFINITY) {
		printf("Warning: Undefined number\n");
		Buffer[0] = '0';
		return 1;
	}

	l = GetLength();

	val = Value;
	if (val < 0) {
		Buffer[0] = '-';
		val *= -1.0;
	}

	i = val;
	d = 10;
	scale = 10e9;
	while (i > 1.0) {
		scale /= 10.0;
		i /= 10.0;
		d--;
	}

	i = floor(val);
	f = round((val - i) * scale);
	while (f >= scale) {
		i++;
		f = round(f - scale);
	}

	j = IntegerPart - 1;
	if (i == 0.0) Buffer[j] = '0';
	while (i >= 1.0) {
		Buffer[j] = mod10(i) + '0';
		j--;
		i = floor(i / 10.0);
	}

	bool b = false;
	j = l - 1;
	if (f > 0.0) {
		Buffer[IntegerPart] = '.';
		while (d) {
			if (b || (mod10(f) > 0)) {
				Buffer[j] = mod10(f) + '0';
				j--;
				b = true;
			}
			f = floor(f / 10.0);
			d--;
		}
	}

	return l;
}


