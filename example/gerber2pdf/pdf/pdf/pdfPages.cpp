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

#include "pdfPages.h"


pdfPages::pdfPages() {
	Object = &Dictionary;
	Type.Set("Pages");

	Parent = 0;
	Update();
}


void pdfPages::Update() {
	Dictionary.Clear();
	Dictionary.AddEntry("Type", &Type);
	Dictionary.AddEntry("Kids", &Children);
	Dictionary.AddEntry("Count", &Count);

	if (Parent) Dictionary.AddEntry("Parent", Parent);
	if (MediaBox.HasArea()) Dictionary.AddEntry("MediaBox", &MediaBox);
	if (Resources.GetCount()) Dictionary.AddEntry("Resources", &Resources);
}


void pdfPages::SetParent(pdfPages* Parent) {
	pdfPages::Parent = Parent;
	Update();
}


void pdfPages::AddChild(pdfPages* Child) {
	Child->SetParent(this);
	Count.Value += Child->Count.Value;
	Children.Add(Child);
	Update();
}


void pdfPages::AddChild(pdfPage* Page) {
	Page->SetParent(this);
	Count.Value++;
	Children.Add(Page);
	Update();
}

