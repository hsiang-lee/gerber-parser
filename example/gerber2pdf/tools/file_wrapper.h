//==============================================================================
// Copyright (C) John-Philip Taylor
// jpt13653903@gmail.com
// This file is part of a library
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//==============================================================================

#ifndef FileWrapper_h
#define FileWrapper_h


#include <vector>


#include "General.h"
#include "UTF_Converter.h"


#if   defined(WINVER)
#elif defined(__linux__)
#else
#error FileWrapper is not implemented for your platform
#endif


class FILE_WRAPPER {
public:
	enum ACCESS {
		faRead,
		faWrite,
		faCreate
	};
private:
#if defined(WINVER)
	HANDLE Handle;
#elif defined(__linux__)
	std::string Filename;
	FILE* Handle;
#endif

	void GetLongName(const wchar_t* Filename, std::wstring& LongName);
	bool CreatePath(const char* Filename);

public:
	FILE_WRAPPER();
	~FILE_WRAPPER();

	bool Open(const  std::string& Filename, ACCESS Access); // UTF-8
	bool Open(const wchar_t* Filename, ACCESS Access); // Windows only: UTF-16
	void Close();

	uint64_t GetSize();

	uint64_t Read(char* buffer_, uint64_t MustRead);
	uint64_t Write(const char* buffer_); // Null-terminated
	uint64_t Write(const char* buffer_, uint64_t MustWrite);
	bool     Flush();

	// Opens, reads and closes the file
	// UTF-8 name; returns null on error; null-terminates the buffer
	// The caller must free the returned buffer with "delete[]"
	_byte* ReadAll(const char* Filename, uint64_t* Filesize = 0);

	// Opens, writes and closes the file
	// UTF-8 name; also creates the path if it does not exist
	// If Size is 0, Data is assumed to be null-terminated
	bool WriteAll(const char* Filename, const byte* Data, uint64_t Size = 0);

#ifdef WINVER
	void GetTime(FILETIME* Creation, FILETIME* Access, FILETIME* Modified);
	void SetTime(
		const FILETIME* Creation,
		const FILETIME* Access,
		const FILETIME* Modified
	);
#else
	void GetTime(time_t* StatusChange, time_t* Access, time_t* Modified);
	void SetTime(
		const time_t* StatusChange,
		const time_t* Access,
		const time_t* Modified
	);
#endif
};


#endif

