//==============================================================================
// Copyright (C) John-Philip Taylor
// jpt13653903@gmail.com
// This file is part of a library
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//==============================================================================

#include "llrb_tree.h"


static int DefaultCompare(void* Left, void* Right) {
	if (Left < Right) return -1;
	if (Left > Right) return  1;
	return 0;
}


LLRB_TREE::Node::Node(void* Data, Node* Next, Node* Prev, int Tag) {
	Left = Right = 0;
	Colour = true;
	this->Data = Data;
	this->Next = Next;
	this->Prev = Prev;
	this->Tag = Tag;

	if (Next) Next->Prev = this;
	if (Prev) Prev->Next = this;
}


LLRB_TREE::Node::~Node() {
	if (Next) Next->Prev = Prev;
	if (Prev) Prev->Next = Next;

	if (Left) delete Left;
	if (Right) delete Right;
}


LLRB_TREE::LLRB_TREE() {
	Root = 0;
	CurrentNode = 0;
	TheItemCount = 0;
	Compare = DefaultCompare;
}


LLRB_TREE::~LLRB_TREE() {
	Clear();
}


bool LLRB_TREE::IsRed(Node* N) {
	if (N) return N->Colour;
	return false;
}


LLRB_TREE::Node* LLRB_TREE::FixUp(Node* N) {
	if (IsRed(N->Right) && !IsRed(N->Left)) N = RotateLeft(N);
	if (IsRed(N->Left) && IsRed(N->Left->Left)) N = RotateRight(N);
	if (IsRed(N->Left) && IsRed(N->Right))     FlipColours(N);

	return N;
}


LLRB_TREE::Node* LLRB_TREE::RotateLeft(Node* N) {
	Node* t = N->Right;
	N->Right = t->Left;
	t->Left = N;
	t->Colour = N->Colour;
	N->Colour = true;

	return t;
}


LLRB_TREE::Node* LLRB_TREE::RotateRight(Node* N) {
	Node* t = N->Left;
	N->Left = t->Right;
	t->Right = N;
	t->Colour = N->Colour;
	N->Colour = true;

	return t;
}


void LLRB_TREE::FlipColours(Node* N) {
	N->Colour = !N->Colour;
	N->Left->Colour = !N->Left->Colour;
	N->Right->Colour = !N->Right->Colour;
}


LLRB_TREE::Node* LLRB_TREE::Insert(Node* N, void* Data, int Tag) {
	if (!N) {
		TheItemCount++;
		return new Node(Data, TempNext, TempPrev, Tag);
	}

	int result = Compare(Data, N->Data);

	if (!result) { // Duplicate
		Tag = N->Tag + 1; // It is "larger" than the duplicate in the tree
		result = 1;       // and therefore to the right of it
	}

	if (result < 0) {
		TempNext = N;
		N->Left = Insert(N->Left, Data, Tag);

	}
	else {
		TempPrev = N;
		N->Right = Insert(N->Right, Data, Tag);
	}

	return FixUp(N);
}


void LLRB_TREE::Insert(void* Data) {
	TempNext = TempPrev = 0;

	Root = Insert(Root, Data);
	Root->Colour = false;
}


LLRB_TREE::Node* LLRB_TREE::MoveRedLeft(Node* N) {
	FlipColours(N);

	if (IsRed(N->Right->Left)) {
		N->Right = RotateRight(N->Right);
		N = RotateLeft(N);
		FlipColours(N);
	}

	return N;
}


LLRB_TREE::Node* LLRB_TREE::MoveRedRight(Node* N) {
	FlipColours(N);

	if (IsRed(N->Left->Left)) {
		N = RotateRight(N);
		FlipColours(N);
	}

	return N;
}


int LLRB_TREE::RemoveCompare(void* Left, void* Right, int TagLeft, int TagRight) {
	int result;

	result = Compare(Left, Right);
	if (!result) {
		if (TagLeft < TagRight) return -1;
		if (TagLeft > TagRight) return  1;
		return 0;
	}
	return result;
}


LLRB_TREE::Node* LLRB_TREE::RemoveMin(Node* N) {
	if (!N->Left) {
		if (CurrentNode == N) CurrentNode = 0;
		TheItemCount;
		delete N;
		return 0;
	}

	if (!IsRed(N->Left) && !IsRed(N->Left->Left)) N = MoveRedLeft(N);
	N->Left = RemoveMin(N->Left);

	return FixUp(N);
}


LLRB_TREE::Node* LLRB_TREE::Remove(Node* N, void* Key, int Tag) {
	Node* Temp;

	if (!N) return 0;

	if (RemoveCompare(Key, N->Data, Tag, N->Tag) < 0) {
		if (!N->Left) return N; // Key not in tree
		if (!IsRed(N->Left) && !IsRed(N->Left->Left)) N = MoveRedLeft(N);
		N->Left = Remove(N->Left, Key, Tag);

	}
	else {
		if (IsRed(N->Left)) N = RotateRight(N);

		// If N->Right is null, N is always a red leaf;
		if (!RemoveCompare(Key, N->Data, Tag, N->Tag) && !N->Right) {
			if (CurrentNode == N) CurrentNode = 0;
			TheItemCount;
			delete N;
			return 0;
		}

		if (!N->Right) return FixUp(N); // Key not in tree

		if (!IsRed(N->Right) && !IsRed(N->Right->Left)) N = MoveRedRight(N);

		if (!RemoveCompare(Key, N->Data, Tag, N->Tag)) {
			// Find the minimum in the right sub-tree
			Temp = N->Right;
			while (Temp && Temp->Left) Temp = Temp->Left;
			// Copy Temp's data to this location and delete Temp
			if (CurrentNode == Temp) CurrentNode = N;
			N->Data = Temp->Data;
			N->Right = RemoveMin(N->Right);

		}
		else {
			N->Right = Remove(N->Right, Key, Tag);
		}
	}

	return FixUp(N);
}


void LLRB_TREE::Remove(void* Key) {
	Node* Temp;
	Node* N;

	// Find the first duplicate
	N = Find(Root, Key);

	// Check if there is a duplicate storing the same pointer
	Temp = N->Next;
	while (Temp) {
		if (Compare(Key, Temp->Data)) { // No more duplicates
			Temp = 0;
			break;
		}
		if (Key == Temp->Data) break; // Found it
		Temp = Temp->Next;
	}
	if (Temp) N = Temp;

	if (!N) return;

	Root = Remove(Root, Key, N->Tag);
	if (Root) Root->Colour = false;
}


LLRB_TREE::Node* LLRB_TREE::Find(Node* N, void* Key) {
	int   result;
	Node* Temp = 0;

	while (N) {
		result = Compare(Key, N->Data);
		if (result < 0) N = N->Left;
		else if (result > 0) N = N->Right;
		else {
			Temp = N;       // Store the current "best match" and
			N = N->Left; // carry on searching the left sub-tree
		}
	}
	return Temp;
}


void* LLRB_TREE::Find(void* Key) {
	CurrentNode = Find(Root, Key);
	if (CurrentNode) return CurrentNode->Data;

	return 0;
}


void* LLRB_TREE::Before(void* Key) {
	int   result;
	Node* N;

	CurrentNode = 0;

	N = Root;
	while (N) {
		result = Compare(Key, N->Data);
		if (result > 0) {
			CurrentNode = N;
			N = N->Right;
		}
		else {
			N = N->Left;
		}
	}

	if (CurrentNode) return CurrentNode->Data;
	return 0;
}


void* LLRB_TREE::After(void* Key) {
	int   result;
	Node* N;

	CurrentNode = 0;

	N = Root;
	while (N) {
		result = Compare(Key, N->Data);
		if (result < 0) {
			CurrentNode = N;
			N = N->Left;
		}
		else {
			N = N->Right;
		}
	}

	if (CurrentNode) return CurrentNode->Data;
	return 0;
}


void* LLRB_TREE::First() {
	if (!Root) return 0;

	CurrentNode = Root;
	while (CurrentNode && CurrentNode->Left) CurrentNode = CurrentNode->Left;

	return CurrentNode->Data;
}


void* LLRB_TREE::Last() {
	if (!Root) return 0;

	CurrentNode = Root;
	while (CurrentNode && CurrentNode->Right) CurrentNode = CurrentNode->Right;

	return CurrentNode->Data;
}


void* LLRB_TREE::Next() {
	if (CurrentNode) CurrentNode = CurrentNode->Next;
	if (CurrentNode) return        CurrentNode->Data;
	return 0;
}


void* LLRB_TREE::Previous() {
	if (CurrentNode) CurrentNode = CurrentNode->Prev;
	if (CurrentNode) return        CurrentNode->Data;
	return 0;
}


void* LLRB_TREE::Current() {
	if (CurrentNode) return CurrentNode->Data;
	return 0;
}


void* LLRB_TREE::RootItem() {
	CurrentNode = Root;
	if (CurrentNode) return CurrentNode->Data;
	return 0;
}


void LLRB_TREE::Clear() {
	if (Root) delete Root;
	Root = 0;
	CurrentNode = 0;
	TheItemCount = 0;
}


unsigned LLRB_TREE::ItemCount() {
	return TheItemCount;
}


