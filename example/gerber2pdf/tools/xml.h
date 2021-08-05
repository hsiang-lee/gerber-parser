//==============================================================================
// Copyright (C) John-Philip Taylor
// jpt13653903@gmail.com
// This file is part of a library
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//==============================================================================

// An abstraction of a subset of the XML-1.0 standard.

// The only supported encoding is UTF-8.

// Content characters "<", ">", """, "'" and "&" are automatically escaped when
// writing and resolved when reading. All other non-null characters are
// considered legal content.

// Illegal characters in names are replaced with underscores.


#ifndef XML_h
#define XML_h


#include <stdio.h>
#include <string.h>


#include "general.h"
#include "llrb_tree.h"
#include "calculator.h"


class XML {
public:
	struct ATTRIBUTE {
		std::string Name;
		std::string Value;
		std::string LegalValue;

		ATTRIBUTE(const char* Name, const char* Value);
		~ATTRIBUTE();
	};

	struct ENTITY {
		std::string Name;
		std::string Comments;
		std::string Content;
		std::string LegalContent;

		LLRB_TREE Children;   // Child entities, sorted by name
		LLRB_TREE Attributes; // Sorted by name

		ENTITY(const char* Name);
		~ENTITY();
	};
	ENTITY* Root; // When changing externally, call Clear() first

private:
	struct NESTING {
		ENTITY* Entity;
		NESTING* Next;

		NESTING(const char* EntityName);
		~NESTING();
	};
	NESTING* Nesting;

	std::string Buffer;
	void GetLegalName(const char* Name, std::string* LegalName);
	void GetLegalContent(std::string* Content, std::string* LegalContent);
	void SaveEntity(ENTITY* Entity, unsigned     Indent = 0);

	CALCULATOR Calc;

	unsigned ReadSize;
	unsigned ReadIndex;
	char* ReadBuffer;

	void PrintError(const char* Message);

	bool    ReadSpace();
	bool    ReadComment();
	bool    ReadSpecial();
	bool    ReadName(std::string* Buffer);
	bool    ReadContent(std::string* Buffer, char End = 0);
	bool    ReadAttribute(LLRB_TREE* Tree);
	bool    ReadHeader();
	ENTITY* ReadEntity();

public:
	XML();
	~XML();

	// Discard all previous data
	void Clear();

	// Discard all previous data and creates a new document,
	// with a top entity named Document
	void New(const char* Document);

	// Nests a new entity under the current one
	void Begin(const char* Entity);

	// Adds a new comment to the current entity
	void Comment(const char* Comment);

	// Adds a new attribute to the current entity
	void Attribute(const char* Name, int          Value); // Decimal
	void Attribute(const char* Name, bool         Value);
	void Attribute(const char* Name, double       Value);
	void Attribute(const char* Name, unsigned     Value); // Hexadecimal
	void Attribute(const char* Name, const char* Value);
	void Attribute(const char* Name, std::string* Value);

	// Adds to the existing content of the current entity
	void Content(int         Value);
	void Content(bool        Value);
	void Content(double      Value);
	void Content(unsigned    Value);
	void Content(const char* Value);

	// Closes the current entity and goes up one nesting level
	// Once the top level entity is closed, no other operations are possible
	void End();

	// Closes all entities on the nesting stack and saves the document to a file
	bool Save(const char* Filename);

	// Discards all previous data and loads the file into the current document
	bool Load(const char* Filename);

	// Finds the entity with name Name (Memory managed by this class)
	ENTITY* FindChild(ENTITY* Entity, const char* Name);
	// After calling FindChild, call this to get the next duplicate
	ENTITY* NextChild(ENTITY* Entity, const char* Name);

	// Finds the attribute with name Name (Memory managed by this class)
	ATTRIBUTE* FindAttribute(ENTITY* Entity, const char* Name);

	// Use these to read attributes directly
	bool ReadAttribute(ENTITY* Entity, const char* Name, int* Value);
	bool ReadAttribute(ENTITY* Entity, const char* Name, bool* Value);
	bool ReadAttribute(ENTITY* Entity, const char* Name, char* Value);
	bool ReadAttribute(ENTITY* Entity, const char* Name, std::string* Value);
	bool ReadAttribute(ENTITY* Entity, const char* Name, double* Value);
	bool ReadAttribute(ENTITY* Entity, const char* Name, unsigned* Value);
};


#endif

