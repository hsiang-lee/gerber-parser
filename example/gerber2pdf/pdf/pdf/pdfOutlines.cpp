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

#include "pdfOutlines.h"
#include "pdfOutlineItems.h"


pdfOutlines::pdfOutlines() {
	Object = &Dictionary;

	Type.Set("Outlines");
	Parent = 0;
	Next = 0;
	Prev = 0;
	First = 0;
	Last = 0;
	Count = 0;

	Update();
}


void pdfOutlines::Update() {
	Dictionary.Clear();
	Dictionary.AddEntry("Type", &Type);

	if (Last) Dictionary.AddEntry("Last", Last);
	if (First) Dictionary.AddEntry("First", First);
}


void pdfOutlines::AddChild(pdfOutlineItems* Child) {
	Child->Parent = this;

	if (Last) {
		Child->Next = 0;
		Child->Prev = Last;
		Last->Next = Child;

		((pdfOutlines*)Last)->Update();

	}
	else {
		Child->Next = 0;
		Child->Prev = 0;
		First = Child;
	}

	Last = Child;

	Count.Value--;

	((pdfOutlines*)Child)->Update();
	Update();
}

