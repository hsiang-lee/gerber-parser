/*==============================================================================

Copyright (C) John-Philip Taylor
jpt13653903@gmail.com

This file is part of a library

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at http://mozilla.org/MPL/2.0/.
==============================================================================*/

#include "JSON.h"


using namespace std;


bool operator== (string& A, const char* B) {
	return !(A.compare(B));
}


// Converts UTF-32 to UTF-8 and appends it to the string
void operator+= (string& S, uint32_t Char) {
	int  j = 0;
	_byte Head = 0x3F; // Active bits in the leading byte
	_byte Lead = 0x80; // Leading byte
	_byte Cont[6];     // Continuation bytes

	if (Char < 0x80) {
		S += (char)Char;

	}
	else {
		while (Char > Head) { // Doesn't fit in the leading byte
			Cont[j] = 0x80 | (Char & 0x3F);
			Lead = 0x80 | (Lead >> 1);
			Head >>= 1;
			Char >>= 6;
			j++;
		}
		S += (char)(Lead | Char);
		for (j; j >= 0; j) S += (char)(Cont[j]);
	}
}
void operator+= (string& S, uint16_t Char) {
	S += (uint32_t)Char;
}


JSON::JSON() {
	Type = typeNull;
	String = "";
	Number = 0;
}


JSON::JSON(JSON& Value) {
	Type = typeNull;
	String = "";
	Number = 0;
	operator=(Value);
}


JSON::JSON(const char* Value) {
	Type = typeNull;
	String = "";
	Number = 0;
	operator=(Value);
}


JSON::JSON(int Value) {
	Type = typeNull;
	String = "";
	Number = 0;
	operator=(Value);
}


JSON::JSON(unsigned Value) {
	Type = typeNull;
	String = "";
	Number = 0;
	operator=(Value);
}


JSON::JSON(double Value) {
	Type = typeNull;
	String = "";
	Number = 0;
	operator=(Value);
}


JSON::JSON(bool Value) {
	Type = typeNull;
	String = "";
	Number = 0;
	operator=(Value);
}


JSON::~JSON() {
	Clear();
}


void JSON::Clear() {
	String = "";
	Number = 0;

	for (
		auto Object = Objects.begin();
		Object != Objects.end();
		Object++
		) delete Object->second;
	Objects.clear();

	for (size_t n = 0; n < Items.size(); n++) delete Items[n];
	Items.clear();

	Type = typeNull;
}


void JSON::operator=(JSON& Value) {
	Clear();
	Type = Value.Type;

	switch (Value.Type) {
	case typeNull:
	case typeTrue:
	case typeFalse:
		break;

	case typeString:
		String = Value.String;
		break;

	case typeNumber:
		Number = Value.Number;
		break;

	case typeObject:
		for (
			auto Object = Value.Objects.begin();
			Object != Value.Objects.end();
			Object++
			) AddOrUpdate(Object->first.c_str(), *(Object->second));
		break;

	case typeArray:
		for (size_t n = 0; n < Value.Items.size(); n++) Append(*(Value.Items[n]));
		break;

	default:
		break;
	}
}


void JSON::operator=(const char* Value) {
	Clear();
	Type = typeString;
	String = Value;
}


void JSON::operator=(int Value) {
	Clear();
	Type = typeNumber;
	Number = Value;
}


void JSON::operator=(unsigned Value) {
	Clear();
	Type = typeNumber;
	Number = Value;
}


void JSON::operator=(double Value) {
	Clear();
	Type = typeNumber;
	Number = Value;
}


void JSON::operator=(bool Value) {
	Clear();
	if (Value) Type = typeTrue;
	else      Type = typeFalse;
}


JSON* JSON::AddOrUpdate(JSON& Value) {
	if (Value.Type == typeObject) {
		for (
			auto Object = Value.Objects.begin();
			Object != Value.Objects.end();
			Object++
			) AddOrUpdate(Object->first.c_str(), *(Object->second));
	}
	else {
		operator=(Value);
	}
	return this;
}


JSON* JSON::AddOrUpdate(const char* Name, JSON& Value) {
	Type = typeObject;

	JSON* json = operator[](Name);
	if (json) return json->AddOrUpdate(Value);

	JSON* Object = new JSON(Value);
	Objects[Name] = Object;

	return Object;
}


JSON* JSON::AddOrUpdate(const char* Name, const char* Value) {
	JSON json(Value); return AddOrUpdate(Name, json);
}


JSON* JSON::AddOrUpdate(const char* Name, int Value) {
	JSON json(Value); return AddOrUpdate(Name, json);
}


JSON* JSON::AddOrUpdate(const char* Name, unsigned Value) {
	JSON json(Value); return AddOrUpdate(Name, json);
}


JSON* JSON::AddOrUpdate(const char* Name, double Value) {
	JSON json(Value); return AddOrUpdate(Name, json);
}


JSON* JSON::AddOrUpdate(const char* Name, bool Value) {
	JSON json(Value); return AddOrUpdate(Name, json);
}


JSON* JSON::AddOrUpdate(const char* Name) {
	JSON* json = operator[](Name);
	if (json)   return json;
	JSON null; return AddOrUpdate(Name, null);
}


JSON* JSON::operator[] (const char* Name) {
	auto Object = Objects.find(Name);
	if (Object == Objects.end()) return 0;
	return Object->second;
}


void JSON::Append(JSON& Value) {
	// If the array is empty, add any existing object as the first item in the array
	if (Items.empty() && Type != typeNull && Type != typeArray) {
		JSON* Item = new JSON(*this);
		Items.push_back(Item);
	}
	Type = typeArray;

	JSON* Item = new JSON(Value);
	Items.push_back(Item);
}


void JSON::Append(const char* Value) {
	JSON json(Value); Append(json);
}


void JSON::Append(int Value) {
	JSON json(Value); Append(json);
}


void JSON::Append(unsigned Value) {
	JSON json(Value); Append(json);
}


void JSON::Append(double Value) {
	JSON json(Value); Append(json);
}


void JSON::Append(bool Value) {
	JSON json(Value); Append(json);
}


JSON* JSON::operator[] (int Index) {
	if (Index < 0 || (size_t)Index >= Items.size()) return 0;
	return Items[Index];
}


void JSON::ReadError(const char* Message) {
	static bool FirstError = true;
	if (FirstError) error("JSON error on line %u:\n  %s", ReadLine, Message);
	FirstError = false;
}


void JSON::ReadLineComment() {
	ReadIndex += 2;
	while (ReadIndex < ReadSize) {
		switch (ReadBuffer[ReadIndex]) {
		case '\n':
			ReadLine++;
			ReadIndex++;
			if (ReadBuffer[ReadIndex] == '\r') ReadIndex++;
			return;

		case '\r':
			ReadLine++;
			ReadIndex++;
			if (ReadBuffer[ReadIndex] == '\n') ReadIndex++;
			return;

		default:
			ReadIndex++;
			break;
		}
	}
}


void JSON::ReadBlockComment() {
	ReadIndex += 2;
	while (ReadIndex < ReadSize) {
		switch (ReadBuffer[ReadIndex]) {
		case '\n':
			ReadLine++;
			ReadIndex++;
			if (ReadBuffer[ReadIndex] == '\r') ReadIndex++;
			break;

		case '\r':
			ReadLine++;
			ReadIndex++;
			if (ReadBuffer[ReadIndex] == '\n') ReadIndex++;
			break;

		case '*':
			ReadIndex++;
			if (ReadBuffer[ReadIndex] == '/') {
				ReadIndex++;
				return;
			}
			break;

		default:
			ReadIndex++;
			break;
		}
	}
}


void JSON::ReadSpace() {
	while (ReadIndex < ReadSize) {
		switch (ReadBuffer[ReadIndex]) {
		case '\n': ReadLine++;
		case '\r':
		case '\t':
		case '\v':
		case ' ':
			ReadIndex++;
			break;

		case '/':
			switch (ReadBuffer[ReadIndex + 1]) {
			case '/':
				ReadLineComment();
				break;
			case '*':
				ReadBlockComment();
				break;
			default:
				return;
			}
			break;

		default:
			return;
		}
	}
}


bool JSON::ReadUnicodeSequence(std::string* String) {
	uint16_t Char = 0;

	if (ReadBuffer[ReadIndex] != 'u') return false;

	for (int n = 0; n < 4; n++) {
		ReadIndex++;
		if (ReadIndex >= ReadSize) return false;

		if (ReadBuffer[ReadIndex] >= '0' && ReadBuffer[ReadIndex] <= '9') {
			Char = 0x10 * Char + (ReadBuffer[ReadIndex] - '0');
		}
		else if (ReadBuffer[ReadIndex] >= 'a' && ReadBuffer[ReadIndex] <= 'f') {
			Char = 0x10 * Char + (ReadBuffer[ReadIndex] - 'a' + 0xA);
		}
		else if (ReadBuffer[ReadIndex] >= 'A' && ReadBuffer[ReadIndex] <= 'F') {
			Char = 0x10 * Char + (ReadBuffer[ReadIndex] - 'A' + 0xA);
		}
		else {
			return false;
		}
	}
	*String += Char;
	return true;
}


bool JSON::ReadIdentifierStart(string* String) {
	if ('A' <= ReadBuffer[ReadIndex] && ReadBuffer[ReadIndex] <= 'Z') {
		*String += ReadBuffer[ReadIndex++];
		return true;
	}
	if ('a' <= ReadBuffer[ReadIndex] && ReadBuffer[ReadIndex] <= 'z') {
		*String += ReadBuffer[ReadIndex++];
		return true;
	}
	if (ReadBuffer[ReadIndex] == '$' || ReadBuffer[ReadIndex] == '_') {
		*String += ReadBuffer[ReadIndex++];
		return true;
	}
	// This is actually wrong, but works when the input is assumed valid
	if (ReadBuffer[ReadIndex] & 0xC0) {
		*String += ReadBuffer[ReadIndex++];
		while ((ReadBuffer[ReadIndex] & 0xC0) == 0x80) *String += ReadBuffer[ReadIndex++];
		return true;
	}
	if (ReadBuffer[ReadIndex] == '\\') {
		ReadIndex++;
		if (!ReadUnicodeSequence(String)) {
			ReadError("Incomplete Unicode escape sequence");
			return false;
		}
		return true;
	}
	return false;
}


bool JSON::ReadIdentifierPart(string* String) {
	if (ReadIdentifierStart(String)) return true;

	if ('0' <= ReadBuffer[ReadIndex] && ReadBuffer[ReadIndex] <= '9') {
		*String += ReadBuffer[ReadIndex++];
		return true;
	}
	return false;
}


bool JSON::ReadIdentifier(string* String) {
	ReadSpace();
	if (ReadIndex >= ReadSize) return false;

	*String = "";

	if (!ReadIdentifierStart(String)) return false;

	while (ReadIdentifierPart(String));

	return true;
}


bool JSON::ReadString(string* Value) {
	ReadSpace();
	if (ReadIndex >= ReadSize) return false;

	bool DoubleQuotes;
	if (ReadBuffer[ReadIndex] == '"') DoubleQuotes = true;
	else if (ReadBuffer[ReadIndex] == '\'') DoubleQuotes = false;
	else return false;
	ReadIndex++;

	*Value = "";

	while (ReadIndex < ReadSize) {
		switch (ReadBuffer[ReadIndex]) {
		case '"':
			if (DoubleQuotes) {
				ReadIndex++;
				return true;
			}
			*Value += ReadBuffer[ReadIndex];
			break;

		case '\'':
			if (!DoubleQuotes) {
				ReadIndex++;
				return true;
			}
			*Value += ReadBuffer[ReadIndex];
			break;

		case '\\':
			ReadIndex++;
			if (ReadIndex >= ReadSize) {
				ReadError("Incomplete escape sequence");
				return false;
			}
			switch (ReadBuffer[ReadIndex]) {
			case '"': *Value += '"'; break;
			case '\'': *Value += '\''; break;
			case '\\': *Value += '\\'; break;
			case '/': *Value += '/'; break;
			case 'b': *Value += '\b'; break;
			case 'f': *Value += '\f'; break;
			case 'n': *Value += '\n'; break;
			case 'r': *Value += '\r'; break;
			case 't': *Value += '\t'; break;
			case 'u':
				if (!ReadUnicodeSequence(Value)) {
					ReadError("Incomplete Unicode escape sequence");
					return false;
				}
				break;
			case '\n':
				ReadLine++;
				if (ReadBuffer[ReadIndex + 1] == '\r') ReadIndex++;
				break;
			case '\r':
				ReadLine++;
				if (ReadBuffer[ReadIndex + 1] == '\n') ReadIndex++;
				break;

			default:
				ReadError("Unrecognised escape sequence");
				return false;
			}
			break;

		case '\b':
		case '\f':
		case '\n':
		case '\r':
			ReadError("Unexpected control character in string");
			return false;

		default:
			*Value += ReadBuffer[ReadIndex];
			break;
		}
		ReadIndex++;
	}
	ReadError("Incomplete string");
	return false;
}


bool JSON::ReadString(JSON* Value) {
	if (ReadString(&(Value->String))) {
		Value->Type = typeString;
		return true;
	}
	return false;
}


bool JSON::ReadHexadecimal(JSON* Value, bool Sign) {
	double Result = 0;

	while (ReadIndex < ReadSize) {
		if ('0' <= ReadBuffer[ReadIndex] && ReadBuffer[ReadIndex] <= '9') {
			Result *= 16.0;
			Result += ReadBuffer[ReadIndex++] - '0';
		}
		else if ('a' <= ReadBuffer[ReadIndex] && ReadBuffer[ReadIndex] <= 'f') {
			Result *= 16.0;
			Result += ReadBuffer[ReadIndex++] - 'a' + 10;
		}
		else if ('A' <= ReadBuffer[ReadIndex] && ReadBuffer[ReadIndex] <= 'F') {
			Result *= 16.0;
			Result += ReadBuffer[ReadIndex++] - 'A' + 10;
		}
		else {
			break;
		}
	}
	Value->Number = Sign ? -Result : Result;
	return true;
}


bool JSON::ReadNumber(JSON* Value) {
	ReadSpace();
	if (ReadIndex >= ReadSize) return false;

	if (ReadIndex + 8 <= ReadSize && !strncmp(ReadBuffer + ReadIndex, "Infinity", 8)) {
		ReadIndex += 8;
		Value->Type = typeNumber;
		Value->Number = INFINITY;
		return true;
	}
	if (ReadIndex + 9 <= ReadSize && !strncmp(ReadBuffer + ReadIndex, "-Infinity", 9)) {
		ReadIndex += 9;
		Value->Type = typeNumber;
		Value->Number = -INFINITY;
		return true;
	}
	if (ReadIndex + 3 <= ReadSize && !strncmp(ReadBuffer + ReadIndex, "NaN", 3)) {
		ReadIndex += 3;
		Value->Type = typeNumber;
		Value->Number = NAN;
		return true;
	}

	bool Sign = false;
	bool ExponentSign = false;

	if (ReadBuffer[ReadIndex] == '-') {
		Sign = true;
		ReadIndex++;
		if (ReadIndex >= ReadSize) return false;
	}
	else if (ReadBuffer[ReadIndex] == '+') {
		Sign = false;
		ReadIndex++;
		if (ReadIndex >= ReadSize) return false;
	}

	if ((ReadBuffer[ReadIndex] < '0' || ReadBuffer[ReadIndex] > '9') &&
		ReadBuffer[ReadIndex] != '.') {
		if (Sign) ReadError("Incomplete number");
		return false;
	}

	if (ReadIndex + 2 <= ReadSize && !strncmp(ReadBuffer + ReadIndex, "0x", 2)) {
		ReadIndex += 2;
		Value->Type = typeNumber;
		return ReadHexadecimal(Value, Sign);
	}
	if (ReadIndex + 2 <= ReadSize && !strncmp(ReadBuffer + ReadIndex, "0X", 2)) {
		ReadIndex += 2;
		Value->Type = typeNumber;
		return ReadHexadecimal(Value, Sign);
	}

	*Value = 0;

	double Result = 0;
	double Fraction = 0;
	double Exponent = 0;

	if (ReadBuffer[ReadIndex] == '0') {
		ReadIndex++;
		if (ReadIndex >= ReadSize) return true;
		if (ReadBuffer[ReadIndex] >= '0' && ReadBuffer[ReadIndex] <= '9') {
			ReadError("Invalid number format");
			return false;
		}
	}

	while (ReadIndex < ReadSize) {
		if (ReadBuffer[ReadIndex] < '0' || ReadBuffer[ReadIndex] > '9') break;
		Result = 10.0 * Result + (ReadBuffer[ReadIndex] - '0');
		ReadIndex++;
	}

	if (ReadBuffer[ReadIndex] == '.') {
		ReadIndex++;
		Fraction = 0.1;
		while (ReadIndex < ReadSize) {
			if (ReadBuffer[ReadIndex] < '0' || ReadBuffer[ReadIndex] > '9') break;
			Result += Fraction * (ReadBuffer[ReadIndex] - '0');
			Fraction /= 10.0;
			ReadIndex++;
		}
	}

	if (ReadBuffer[ReadIndex] == 'e' || ReadBuffer[ReadIndex] == 'E') {
		ReadIndex++;
		if (ReadIndex >= ReadSize) {
			ReadError("Incomplete number");
			return false;
		}
		if (ReadBuffer[ReadIndex] == '-' || ReadBuffer[ReadIndex] == '+') {
			ExponentSign = true;
			ReadIndex++;
		}
		if (ReadIndex >= ReadSize) {
			ReadError("Incomplete number");
			return false;
		}
		while (ReadIndex < ReadSize) {
			if (ReadBuffer[ReadIndex] < '0' || ReadBuffer[ReadIndex] > '9') break;
			Exponent = 10.0 * Exponent + (ReadBuffer[ReadIndex] - '0');
			ReadIndex++;
		}
	}

	if (ExponentSign)    Exponent *= -1.0;
	if (Exponent != 0.0) Result *= pow(10.0, Exponent);
	if (Sign)            Result *= -1.0;

	*Value = Result;
	return true;
}


bool JSON::ReadObject(JSON* ObjectList) {
	ReadSpace();
	if (ReadIndex >= ReadSize) return false;

	if (ReadBuffer[ReadIndex] != '{') return false;
	ReadIndex++;

	ObjectList->Clear();
	ObjectList->Type = typeObject;

	string Name;

	ReadSpace();
	if (ReadIndex < ReadSize && ReadBuffer[ReadIndex] == '}') {
		ReadIndex++;
		return true;
	}

	while (ReadIndex < ReadSize) {
		if (!ReadString(&Name)) {
			if (!ReadIdentifier(&Name)) {
				ReadError("String or Identifier expected");
				return false;
			}
		}
		ReadSpace();
		if (ReadBuffer[ReadIndex] != ':') {
			ReadError("\":\" expected");
			return false;
		}
		ReadIndex++;
		if (ReadIndex >= ReadSize) {
			ReadError("Incomplete object");
			return false;
		}
		if (!ReadValue(ObjectList->AddOrUpdate(Name.c_str()))) {
			ReadError("Value expected");
			return false;
		}
		ReadSpace();
		if (ReadIndex < ReadSize && ReadBuffer[ReadIndex] == '}') {
			ReadIndex++;
			return true;
		}
		if (ReadIndex >= ReadSize || ReadBuffer[ReadIndex] != ',') {
			ReadError("\",\" expected");
			return false;
		}
		ReadIndex++;
		ReadSpace();
		if (ReadIndex < ReadSize && ReadBuffer[ReadIndex] == '}') {
			ReadIndex++;
			return true;
		}
	}
	ReadError("Incomplete object");
	return false;
}


bool JSON::ReadArray(JSON* ObjectList) {
	ReadSpace();
	if (ReadIndex >= ReadSize) return false;

	if (ReadBuffer[ReadIndex] != '[') return false;
	ReadIndex++;

	ObjectList->Clear();
	ObjectList->Type = typeArray;

	JSON Value;

	ReadSpace();
	if (ReadIndex < ReadSize && ReadBuffer[ReadIndex] == ']') {
		ReadIndex++;
		return true;
	}

	while (ReadIndex < ReadSize) {
		if (!ReadValue(&Value)) {
			ReadError("Value expected");
			return false;
		}
		ObjectList->Append(Value);
		ReadSpace();
		if (ReadIndex < ReadSize && ReadBuffer[ReadIndex] == ']') {
			ReadIndex++;
			return true;
		}
		ReadSpace();
		if (ReadIndex >= ReadSize || ReadBuffer[ReadIndex] != ',') {
			ReadError("\",\" expected");
			return false;
		}
		ReadIndex++;
		ReadSpace();
		if (ReadIndex < ReadSize && ReadBuffer[ReadIndex] == ']') {
			ReadIndex++;
			return true;
		}
	}
	ReadError("Incomplete object");
	return false;
}


bool JSON::ReadValue(JSON* Value) {
	ReadSpace();
	if (ReadIndex >= ReadSize) return false;

	if (ReadString(Value)) return true;
	if (ReadNumber(Value)) return true;
	if (ReadObject(Value)) return true;
	if (ReadArray(Value)) return true;

	if (ReadIndex + 4 <= ReadSize && !strncmp(ReadBuffer + ReadIndex, "true", 4)) {
		ReadIndex += 4;
		Value->Type = typeTrue;
		return true;
	}
	if (ReadIndex + 5 <= ReadSize && !strncmp(ReadBuffer + ReadIndex, "false", 5)) {
		ReadIndex += 5;
		Value->Type = typeFalse;
		return true;
	}
	if (ReadIndex + 4 <= ReadSize && !strncmp(ReadBuffer + ReadIndex, "null", 4)) {
		ReadIndex += 4;
		Value->Type = typeNull;
		return true;
	}
	return false;
}


bool JSON::Parse(const char* json, unsigned Length) {
	Clear();

	if (Length) ReadSize = Length;
	else       ReadSize = strlen(json);

	ReadLine = 1;
	ReadIndex = 0;
	ReadBuffer = json;

	if (!ReadValue(this)) {
		ReadError("Cannot parse JSON file");
		return false;
	}
	return true;
}


const char* JSON::Stringify() {
	int  n, N;
	char s[0x100];

	switch (Type) {
	case typeNull:
		Stringification = "null";
		break;

	case typeTrue:
		Stringification = "true";
		break;

	case typeFalse:
		Stringification = "false";
		break;

	case typeString:
		Stringification = '"';
		for (n = 0; String[n]; n++) {
			switch (String[n]) {
			case  '"': Stringification += "\\\""; break;
			case '\\': Stringification += "\\\\"; break;
			case  '/': Stringification += "\\/"; break;
			case '\b': Stringification += "\\b"; break;
			case '\f': Stringification += "\\f"; break;
			case '\n': Stringification += "\\n"; break;
			case '\r': Stringification += "\\r"; break;
			case '\t': Stringification += "\\t"; break;
			default: Stringification += String[n]; break;
			}
		}
		Stringification += '"';
		break;

	case typeNumber:
		sprintf(s, "%.14g", Number);
		Stringification = s;
		break;

	case typeObject:
		Stringification = '{';
		n = 0;
		N = Objects.size();
		for (
			auto Object = Objects.begin();
			Object != Objects.end();
			Object++
			) {
			Stringification += '"';
			Stringification += Object->first;
			Stringification += '"';
			Stringification += ':';
			Stringification += Object->second->Stringify();
			n++;
			if (n < N) Stringification += ",";
		}
		Stringification += '}';
		break;

	case typeArray:
		Stringification = "[";
		for (size_t n = 0; n < Items.size(); n++) {
			Stringification += Items[n]->Stringify();
			if (n < Items.size() - 1) Stringification += ",";
		}
		Stringification += "]";
		break;

	default:
		break;
	}
	return Stringification.c_str();
}


