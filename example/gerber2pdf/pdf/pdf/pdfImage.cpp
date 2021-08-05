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

#include "pdfImage.h"


pdfImage::pdfImage(const char* Name) {
	Type.Set("XObject");
	Subtype.Set("Image");
	ColourSpace.Set("DeviceRGB");
	Interpolate.Value = true;

	pdfImage::Name.Set(Name);

	BitsPerColour = 8;

	Update();
}


void pdfImage::Update() {
	Dictionary.Clear();
	Dictionary.AddEntry("Type", &Type);
	Dictionary.AddEntry("Subtype", &Subtype);
	Dictionary.AddEntry("Name", &Name);
	Dictionary.AddEntry("Width", &Width);
	Dictionary.AddEntry("Height", &Height);
	Dictionary.AddEntry("ColorSpace", &ColourSpace);
	Dictionary.AddEntry("BitsPerComponent", &BitsPerColour);
	Dictionary.AddEntry("Interpolate", &Interpolate);
	Dictionary.AddEntry("Length", &Length);

	if (!Filter.Empty())  Dictionary.AddEntry("Filter", &Filter);
}

