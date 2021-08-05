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

#include "pdfResources.h"
#include "pdfForm.h"


pdfResources::pdfResources() {
	Indent = 1;
	ExtGState.OneLine = true;
	XObjects.OneLine = true;
	Fonts.OneLine = true;
}


void pdfResources::Update() {
	Clear();
	if (Fonts.GetCount()) AddEntry("Font", &Fonts);
	if (XObjects.GetCount()) AddEntry("XObject", &XObjects);
	if (ExtGState.GetCount()) AddEntry("ExtGState", &ExtGState);
}


void pdfResources::AddFont(pdfFont* Font) {
	Fonts.AddEntry(Font->Name.Get(), Font);
	Update();
}


void pdfResources::AddForm(pdfForm* Form) {
	XObjects.AddEntry(Form->Name.Get(), Form);
	Update();
}


void pdfResources::AddImage(pdfImage* Image) {
	XObjects.AddEntry(Image->Name.Get(), Image);
	Update();
}


void pdfResources::AddOpaque(pdfOpaque* Opaque) {
	ExtGState.AddEntry(Opaque->Name.Get(), Opaque);
	Update();
}


