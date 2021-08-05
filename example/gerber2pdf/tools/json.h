/*==============================================================================
Copyright (C) John-Philip Taylor
jpt13653903@gmail.com

This file is part of a library

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
==============================================================================*/

#ifndef JSON_h
#define JSON_h


#include <map>
#include <string>
#include <vector>


#include "General.h"


class JSON {
public: // Object type
	enum TYPE {
		typeNull, typeTrue, typeFalse,
		typeString, typeNumber,
		typeObject, typeArray,
	} Type;


public: // Base types
	std::string String;
	double      Number;

	// These functions make copies of the value
	void operator=(const char* Value);
	void operator=(int         Value);
	void operator=(unsigned    Value);
	void operator=(double      Value);
	void operator=(bool        Value);


public: // Object-related functions
	std::map<std::string, JSON*> Objects;

	// Adds a new key-value pair, or updates the existing
	// - If "Value" is "typeObject", the update is recursive (i.e. old
	//   values are not deleted, while new ones are added or updated)
	// - These functions make copies of the value.
	// - Returns the JSON object containing the value.
	// - If no "Value" is provided, it either creates a new object with
	//   value "null", or returns the value of the current object.
	// - If no "Name" is provided, the update operation is on the root object
	JSON* AddOrUpdate(JSON& Value);
	JSON* AddOrUpdate(const char* Name, JSON& Value);
	JSON* AddOrUpdate(const char* Name, const char* Value);
	JSON* AddOrUpdate(const char* Name, int         Value);
	JSON* AddOrUpdate(const char* Name, unsigned    Value);
	JSON* AddOrUpdate(const char* Name, double      Value);
	JSON* AddOrUpdate(const char* Name, bool        Value);
	JSON* AddOrUpdate(const char* Name);

	// Cannot return a reference, because it could be null (i.e. not in the list)
	JSON* operator[] (const char* Name);


public: // Array-related functions
	std::vector<JSON*> Items;

	// These functions make copies of the value
	// If the type is not "null" and not "Array", the current value becomes
	// the first item of the array and the provided "Value" the second.
	void Append(JSON& Value);
	void Append(const char* Value);
	void Append(int         Value);
	void Append(unsigned    Value);
	void Append(double      Value);
	void Append(bool        Value);

	// Cannot return a reference, because it could be null (i.e. not in the array)
	JSON* operator[] (int Index);


private: // Private stuff
	std::string Stringification; // Used to return from Stringify()


private: // Parser
	const char* ReadBuffer;
	unsigned    ReadLine;
	unsigned    ReadSize;
	unsigned    ReadIndex;

	void ReadError(const char* Message);
	void ReadSpace();
	void ReadLineComment();
	void ReadBlockComment();

	bool ReadUnicodeSequence(std::string* String);
	bool ReadIdentifierStart(std::string* String);
	bool ReadIdentifierPart(std::string* String);

	bool ReadHexadecimal(JSON* Value, bool Sign);
	bool ReadIdentifier(std::string* Value);
	bool ReadString(std::string* Value);
	bool ReadString(JSON* Value);
	bool ReadNumber(JSON* Value);
	bool ReadObject(JSON* ObjectList);
	bool ReadArray(JSON* ObjectList);
	bool ReadValue(JSON* Value);


public:
	JSON();
	JSON(JSON& Value);
	JSON(const char* Value);
	JSON(int         Value);
	JSON(unsigned    Value);
	JSON(double      Value);
	JSON(bool        Value);
	~JSON();

	// If "Length" is 0, "strlen" is used to determine the length
	bool Parse(const char* json, unsigned Length = 0);

	// This function makes a copy of the contents
	void operator=(JSON& json);

	// Discard all previous data
	void Clear();

	// Converts to a JSON string (internal allocation, do not free)
	const char* Stringify();
};


#endif

