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

#include "pdfPage.h"


pdfPage::pdfPage(bool UseCMYK) {
	Object = &Dictionary;
	Type.Set("Page");

	GroupSubtype.Set("Transparency");
	if (UseCMYK) GroupColourSpace.Set("DeviceCMYK");
	else        GroupColourSpace.Set("DeviceRGB");

	Group.AddEntry("S", &GroupSubtype);
	Group.AddEntry("CS", &GroupColourSpace);
	Group.OneLine = true;

	Parent = 0;
	ContentsPointer = 0;
	Update();
}


void pdfPage::Update() {
	Dictionary.Clear();
	Dictionary.AddEntry("Type", &Type);
	Dictionary.AddEntry("Group", &Group);
	if (Parent) Dictionary.AddEntry("Parent", Parent);
	if (Annotations.GetCount()) Dictionary.AddEntry("Annots", &Annotations);
	if (MediaBox.HasArea()) Dictionary.AddEntry("MediaBox", &MediaBox);
	if (ContentsPointer) Dictionary.AddEntry("Contents", ContentsPointer);
	if (Resources.GetCount()) Dictionary.AddEntry("Resources", &Resources);
}


void pdfPage::AddAnnotation(pdfAnnotation* Annotation) {
	Annotations.Add(Annotation);
}


void pdfPage::SetParent(pdfPages* Parent) {
	pdfPage::Parent = Parent;
	Update();
}


void pdfPage::Contents(pdfContents* Contents) {
	ContentsPointer = Contents;
	Update();
}

