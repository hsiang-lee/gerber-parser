/*==============================================================================

Copyright (C) John-Philip Taylor
jpt13653903@gmail.com

This file is part of a library

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
==============================================================================*/

#include "Dictionary.h"


DICTIONARY_BASE::NODE::NODE(const char* Name, void* Data, NODE* Prev, NODE* Next) {
	Red = true;
	left_ = Right = 0;

	int j;
	for (j = 0; Name[j]; j++);
	this->Name = new char[j + 1];
	for (j = 0; Name[j]; j++) this->Name[j] = Name[j];
	this->Name[j] = 0;

	this->Data = Data;
	this->Prev = Prev;
	this->Next = Next;

	if (Prev) Prev->Next = this;
	if (Next) Next->Prev = this;
}


DICTIONARY_BASE::NODE::~NODE() {
	if (Prev) Prev->Next = Next;
	if (Next) Next->Prev = Prev;

	delete[] Name;

	if (left_) delete left_;
	if (Right) delete Right;
}


void* DICTIONARY_BASE::DefaultOnDuplicate(const char* Name, void* Old, void* New) {
	return New;
}


DICTIONARY_BASE::DICTIONARY_BASE() {
	Root = 0;
	Current = 0;
	ItemCount = 0;
	OnDuplicate = DefaultOnDuplicate;
}


DICTIONARY_BASE::~DICTIONARY_BASE() {
	if (Root) delete Root;
}


void DICTIONARY_BASE::Clear() {
	if (Root) delete Root;
	Root = 0;
	Current = 0;
	ItemCount = 0;
}


bool DICTIONARY_BASE::IsRed(NODE* Node) {
	if (!Node) return false;
	return Node->Red;
}


void DICTIONARY_BASE::ColourFlip(NODE* Node) {
	Node->Red = !Node->Red;
	Node->left_->Red = !Node->left_->Red;
	Node->Right->Red = !Node->Right->Red;
}


DICTIONARY_BASE::NODE* DICTIONARY_BASE::RotateLeft(NODE* Node) {
	NODE* Temp = Node->Right;
	Node->Right = Temp->left_;
	Temp->left_ = Node;
	Temp->Red = Node->Red;
	Node->Red = true;
	return Temp;
}


DICTIONARY_BASE::NODE* DICTIONARY_BASE::RotateRight(NODE* Node) {
	NODE* Temp = Node->left_;
	Node->left_ = Temp->Right;
	Temp->Right = Node;
	Temp->Red = Node->Red;
	Node->Red = true;
	return Temp;
}


void DICTIONARY_BASE::Insert(const char* Name, void* Data) {
	TempPrev = TempNext = 0;
	Root = Insert(Root, Name, Data);
}


DICTIONARY_BASE::NODE* DICTIONARY_BASE::Insert(
	NODE* Node,
	const char* Name,
	void* Data
) {
	if (!Node) {
		ItemCount++;
		return new NODE(Name, Data, TempPrev, TempNext);
	}

	int j;
	for (j = 0; Name[j]; j++) {
		if (Name[j] < Node->Name[j]) {
			TempNext = Node;
			Node->left_ = Insert(Node->left_, Name, Data);
			break;

		}
		else if (Name[j] > Node->Name[j]) {
			TempPrev = Node;
			Node->Right = Insert(Node->Right, Name, Data);
			break;
		}
	}
	if (!Name[j]) {
		if (!Node->Name[j]) {
			Node->Data = OnDuplicate(Name, Node->Data, Data);
			return Node;
		}
		else {
			TempNext = Node;
			Node->left_ = Insert(Node->left_, Name, Data);
		}
	}

	if (IsRed(Node->Right) && !IsRed(Node->left_)) Node = RotateLeft(Node);
	if (IsRed(Node->left_) && IsRed(Node->left_->left_)) Node = RotateRight(Node);
	if (IsRed(Node->left_) && IsRed(Node->Right))        ColourFlip(Node);

	return Node;
}


void* DICTIONARY_BASE::Find(const char* Name) {
	int   j;
	NODE* Node = Root;

	while (Node) {
		if (Name[0] < Node->Name[0]) {
			Node = Node->left_;

		}
		else if (Name[0] > Node->Name[0]) {
			Node = Node->Right;

		}
		else {
			for (j = 1; Name[j]; j++) {
				if (Name[j] < Node->Name[j]) {
					Node = Node->left_;
					break;

				}
				else if (Name[j] > Node->Name[j]) {
					Node = Node->Right;
					break;
				}
			}
			if (!Name[j]) {
				if (!Node->Name[j]) {
					Current = Node;
					return Node->Data;
				}
				else {
					Node = Node->left_;
				}
			}
		}
	}
	return 0;
}


void* DICTIONARY_BASE::First(const char** Name) {
	Current = Root;
	if (!Current) return 0;
	while (Current->left_) Current = Current->left_;
	if (Name) *Name = Current->Name;
	return Current->Data;
}


void* DICTIONARY_BASE::Next(const char** Name) {
	if (!Current) return 0;
	Current = Current->Next;
	if (!Current) return 0;
	if (Name) *Name = Current->Name;
	return Current->Data;
}


int DICTIONARY_BASE::GetCount() {
	return ItemCount;
}


void DICTIONARY_BASE::Action(ACTION Function) {
	if (Root) Action(Root, Function);
}


void DICTIONARY_BASE::Action(NODE* Node, ACTION Function) {
	if (Node->left_) Action(Node->left_, Function);
	Function(Node->Name, Node->Data);
	if (Node->Right) Action(Node->Right, Function);
}

