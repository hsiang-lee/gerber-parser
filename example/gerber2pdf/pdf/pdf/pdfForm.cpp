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

#include "pdfForm.h"


pdfForm::pdfForm(const char* Name, bool UseCMYK) : pdfContents(UseCMYK) {
	Type.Set("XObject");
	SubType.Set("Form");

	GroupSubtype.Set("Transparency");

	Group.AddEntry("S", &GroupSubtype);
	Group.OneLine = true;

	pdfForm::Name.Set(Name);

	Update();
}


void pdfForm::Update() {
	Dictionary.Clear();
	Dictionary.AddEntry("Type", &Type);
	Dictionary.AddEntry("Subtype", &SubType);
	Dictionary.AddEntry("BBox", &BBox);
	Dictionary.AddEntry("Group", &Group);
	Dictionary.AddEntry("Length", &Length);

	if (!Filter.Empty()) Dictionary.AddEntry("Filter", &Filter);
	if (Resources.GetCount()) Dictionary.AddEntry("Resources", &Resources);
}

