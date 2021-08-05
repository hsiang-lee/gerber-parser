/*==============================================================================

Copyright (C) John-Philip Taylor
jpt13653903@gmail.com

This file is part of a library

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.


This is a dictionary to store arbitrary data with a string ID (also known as
an associative array).  It is based on a left-leaning red-black tree, as
described by Robert Sedgewick, Department of Computer Science, Princeton
University, Princeton, NJ 08544
==============================================================================*/

#ifndef Dictionary_h
#define Dictionary_h


class DICTIONARY_BASE {
public:
	typedef void  (*ACTION)(const char* Name, void* Data);
	typedef void* (*ON_DUPLICATE)(const char* Name, void* Old, void* New);

private:
	struct NODE {
		bool  Red;

		char* Name; ///< This memory is internally managed
		void* Data; ///< This is arbitrary data and not deleted by this structure

		// Used to point to the children
		NODE* left_;
		NODE* Right;

		// Used to linearise the tree
		NODE* Prev;
		NODE* Next;

		NODE(const char* Name, void* Data, NODE* Prev, NODE* Next);
		~NODE();
	};

	NODE* Root;
	NODE* Current; // Used in "First" and "Next" calls
	NODE* TempPrev;
	NODE* TempNext;
	int   ItemCount;

	bool  IsRed(NODE* Node);
	void  ColourFlip(NODE* Node);
	NODE* RotateLeft(NODE* Node);
	NODE* RotateRight(NODE* Node);

	NODE* Insert(NODE* Node, const char* Name, void* Data);
	void  Action(NODE* Node, ACTION Function);

	static void* DefaultOnDuplicate(const char* Name, void* Old, void* New);

protected:
	void  Clear();
	void  Insert(const char* Name, void* Data);
	void* Find(const char* Name);
	void* First(const char** Name = 0);
	void* Next(const char** Name = 0);

public:
	DICTIONARY_BASE();
	virtual ~DICTIONARY_BASE();

	// Callback function called upon duplicate insert.  The return value must
	// be the data that must be stored at that location.  The default behaviour
	// is to update to the new data, without calling "delete" or similar.
		// NOTE: The template class below does call "delete"
	ON_DUPLICATE OnDuplicate;

	int GetCount();

	// This calls the given function for every node, in order
	void Action(ACTION Function);
};


template<class type> class DICTIONARY : public DICTIONARY_BASE {
private:
	static type* DefaultOnDuplicate(const char* Name, type* Old, type* New) {
		delete Old;
		return New;
	}
	static void CleanupAction(const char* Name, type* Data) {
		delete Data;
	}

public:
	DICTIONARY() {
		OnDuplicate = (ON_DUPLICATE)DefaultOnDuplicate;
	}
	virtual ~DICTIONARY() {
		Action((ACTION)CleanupAction);
	}
	void Clear() {
		Action((ACTION)CleanupAction);
		DICTIONARY_BASE::Clear();
	}
	void Insert(const char* Name, type* Data) {
		DICTIONARY_BASE::Insert(Name, Data);
	}
	type* Find(const char* Name) {
		return (type*)DICTIONARY_BASE::Find(Name);
	}
	type* First(const char** Name = 0) {
		return (type*)DICTIONARY_BASE::First(Name);
	}
	type* Next(const char** Name = 0) {
		return (type*)DICTIONARY_BASE::Next(Name);
	}
};


#endif


