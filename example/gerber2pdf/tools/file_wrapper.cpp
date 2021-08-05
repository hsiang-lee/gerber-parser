//==============================================================================
// Copyright (C) John-Philip Taylor
// jpt13653903@gmail.com
// This file is part of a library
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//==============================================================================

#include "file_wrapper.h"


#ifdef __linux__
#include <utime.h>
#endif


using namespace std;


#ifndef WINVER
#define INVALID_HANDLE_VALUE 0
#define FILETIME time_t
#endif


FILE_WRAPPER::FILE_WRAPPER() {
	Handle = INVALID_HANDLE_VALUE;
}


FILE_WRAPPER::~FILE_WRAPPER() {
	Close();
}


void FILE_WRAPPER::GetLongName(const wchar_t* Filename, wstring& LongName) {
#ifdef WINVER
	// Extend the Path to use long names
	if (Filename[1] == L':' && Filename[2] == L'\\') LongName = L"\\\\?\\";

	for (int n = 0; Filename[n]; n++) {
		if (Filename[n] == L'/') LongName += L'\\';
		else                    LongName += Filename[n];
	}
#else
	// Other operating systems don't have this problem
	LongName = Filename;
#endif
}


bool FILE_WRAPPER::Open(const std::string& Filename, ACCESS Access) {
	if (Filename.empty() || !Filename[0]) return false;

#ifdef WINVER
	return Open((const wchar_t*)UTF_Converter.UTF16(Filename).c_str(), Access);

#else
	if (Handle) Close();
	this->Filename = Filename;

	switch (Access) {
	case faRead:
		Handle = fopen(Filename, "rb");
		break;

	case faWrite:
		Handle = fopen(Filename, "wb");
		if (Handle) fseek(Handle, 0, SEEK_END);
		break;

	case faCreate:
		Handle = fopen(Filename, "wb");
		break;

	default:
		break;
	}
	return Handle;
#endif
}


bool FILE_WRAPPER::Open(const wchar_t* Filename, ACCESS Access) {
#ifdef WINVER
	if (!Filename || !Filename[0]) return false;

	if (Handle != INVALID_HANDLE_VALUE) Close();

	DWORD DesiredAccess = 0;
	DWORD ShareMode = 0;
	DWORD Creation = 0;

	switch (Access) {
	case faRead:
		DesiredAccess = GENERIC_READ;
		ShareMode = FILE_SHARE_READ;
		Creation = OPEN_EXISTING;
		break;

	case faWrite:
		DesiredAccess = GENERIC_WRITE;
		Creation = OPEN_EXISTING;
		break;

	case faCreate:
		DesiredAccess = GENERIC_WRITE;
		Creation = CREATE_ALWAYS;
		break;

	default:
		break;
	}

	// TODO: Long names (i.e. prefixed with "\\?\" cannot have relative paths.
	//       Relative paths should therefore be resolved if long names are
	//       required.
	// wstring LongName;
	// GetLongName(Filename, LongName);

	Handle = CreateFileW(
		Filename,
		DesiredAccess,
		ShareMode,
		0,
		Creation,
		FILE_ATTRIBUTE_NORMAL,
		0
	);

	// Try again if failed with CREATE_ALWAYS  This is a Windows 10 bug
	if (Handle == INVALID_HANDLE_VALUE && Creation == CREATE_ALWAYS) {
		Handle = CreateFileW(
			Filename,
			DesiredAccess,
			ShareMode,
			0,
			TRUNCATE_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			0
		);
	}

	if (Handle == INVALID_HANDLE_VALUE) return false;

	if (Access == faWrite) SetFilePointer(Handle, 0, 0, FILE_END);

	return true;

#else // Not Windows
	return false;
#endif
}


void FILE_WRAPPER::Close() {
#ifdef WINVER
	if (Handle == INVALID_HANDLE_VALUE) return;
	CloseHandle(Handle);
	Handle = INVALID_HANDLE_VALUE;
#else
	if (Handle) fclose(Handle);
	Handle = 0;
#endif
}


uint64_t FILE_WRAPPER::GetSize() {
	if (Handle == INVALID_HANDLE_VALUE) return 0;

#if defined(WINVER)
	uint64_t result;
	DWORD    high = 0;

	result = GetFileSize(Handle, &high);
	result += ((uint64_t)high) << 32;
	return result;

#elif defined(__linux__)
	long int position = ftell(Handle);
	fseek(Handle, 0, SEEK_END);
	long int size = ftell(Handle);
	fseek(Handle, position, SEEK_SET);
	return size;
#endif
}


uint64_t FILE_WRAPPER::Read(char* buffer_, uint64_t MustRead) {
	if (Handle == INVALID_HANDLE_VALUE) return 0;

#ifdef WINVER
	DWORD Length;
	if (!ReadFile(Handle, buffer_, MustRead, &Length, 0)) return 0;
	return Length;
#else
	return fread(Buffer, 1, MustRead, Handle);
#endif
}


uint64_t FILE_WRAPPER::Write(const char* buffer_) {
	int j;
	for (j = 0; buffer_[j]; j++);
	return Write(buffer_, j);
}


uint64_t FILE_WRAPPER::Write(const char* buffer_, uint64_t MustWrite) {
	if (Handle == INVALID_HANDLE_VALUE) return 0;

#ifdef WINVER
	DWORD Length;
	if (!WriteFile(Handle, buffer_, MustWrite, &Length, 0)) return 0;
	return Length;
#else
	return fwrite(Buffer, 1, MustWrite, Handle);
#endif
}


bool FILE_WRAPPER::Flush() {
	if (Handle == INVALID_HANDLE_VALUE) return false;

#ifdef WINVER
	return FlushFileBuffers(Handle);
#else
	return !fflush(Handle);
#endif
}


_byte* FILE_WRAPPER::ReadAll(const char* Filename, uint64_t* Filesize) {
	if (!Open(Filename, faRead)) return 0;
	uint64_t Size = GetSize();
	if (Filesize) *Filesize = Size;

	_byte* buffer_ = new _byte[Size + 1];
	if (!buffer_) return 0;

	if (Read((char*)buffer_, Size) != Size) {
		delete[] buffer_;
		Close();
		return 0;
	}
	buffer_[Size] = 0;

	Close();
	return buffer_;
}


bool FILE_WRAPPER::CreatePath(const char* Filename) {
#ifdef WINVER
	wstring Path;
	GetLongName((const wchar_t*)UTF_Converter.UTF16(Filename).c_str(), Path);

	vector<int> Slashes;

	int n;
	for (n = 0; Path[n]; n++) {
		if (Path[n] == L'\\') Slashes.push_back(n);
	}
	if (Slashes.empty()) return true;

	int  s;
	bool Exists;

	n = Slashes.size() - 1;
	while (0 <= n && n < (int)Slashes.size()) {
		s = Slashes[n];
		Path[s] = 0;
		Exists = CreateDirectory(Path.c_str(), 0);
		if (!Exists) Exists = (GetLastError() == ERROR_ALREADY_EXISTS);
		Path[s] = '\\';

		if (Exists) n++;
		else       n;
	}
	return n >= 0;

#else
	string Path = Filename;

	vector<int> Slashes;

	int n;
	for (n = 0; Path[n]; n++) {
		if (Path[n] == '/') Slashes.push_back(n);
	}
	if (Slashes.empty()) return true;

	int  s;
	bool Exists;

	n = Slashes.size() - 1;
	while (0 <= n && n < (int)Slashes.size()) {
		s = Slashes[n];
		Path[s] = 0;
		Exists = (mkdir(Path.c_str(), 0777) == 0);
		if (!Exists) Exists = (errno == EEXIST);
		Path[s] = '/';

		if (Exists) n++;
		else       n;
	}
	return n >= 0;
#endif
}


bool FILE_WRAPPER::WriteAll(const char* Filename, const _byte* Data, uint64_t Size) {
	if (!Filename || !Filename[0]) return false;
	if (!CreatePath(Filename)) return false;
	if (!Open(Filename, faCreate)) return false;

	if (!Size) for (Size = 0; Data[Size]; Size++);

	bool Result = (Write((const char*)Data, Size) == Size);

	Close();
	return Result;
}


void FILE_WRAPPER::GetTime(
	FILETIME* Creation,
	FILETIME* Access,
	FILETIME* Modified
) {
	if (Handle == INVALID_HANDLE_VALUE) return;

#ifdef WINVER
	GetFileTime(Handle, Creation, Access, Modified);
#else
	struct stat Stat;
	if (!stat(Filename.c_str(), &Stat)) {
		*Creation = Stat.st_ctime;
		*Access = Stat.st_atime;
		*Modified = Stat.st_mtime;
	}
#endif
}


void FILE_WRAPPER::SetTime(
	const FILETIME* Creation,
	const FILETIME* Access,
	const FILETIME* Modified
) {
	if (Handle == INVALID_HANDLE_VALUE) return;

#ifdef WINVER
	SetFileTime(Handle, Creation, Access, Modified);
#else
	utimbuf Time;
	Time.actime = *Access;
	Time.modtime = *Modified;
	utime(Filename.c_str(), &Time);
#endif
}

