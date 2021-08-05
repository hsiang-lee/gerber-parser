//==============================================================================
// Copyright (C) John-Philip Taylor
// jpt13653903@gmail.com
// This file is part of a library
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//==============================================================================

#include "General.h"


void SetupTerminal() {
#ifdef WINVER
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

	HANDLE TerminalHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (TerminalHandle == INVALID_HANDLE_VALUE) {
		error("Invalid terminal handle");
		return;
	}

	// Set output mode to handle ANSI terminal sequences
	DWORD dwMode = 0;
	if (!GetConsoleMode(TerminalHandle, &dwMode)) {
		// This is OK, because the terminal in question is
		// probably a MinTTY (or similar)
		return;
	}
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(TerminalHandle, dwMode)) {
		error("Unable to set terminal mode");
		return;
	}

	// Set the console encoding to UTF-8
	if (!SetConsoleOutputCP(CP_UTF8)) {
		error("Unable to set terminal to UTF-8");
		return;
	}

	// Make the console buffer as many lines as the system will allow
	CONSOLE_SCREEN_BUFFER_INFO Info;
	GetConsoleScreenBufferInfo(TerminalHandle, &Info);
	Info.dwSize.Y = 0x7FFF;
	while (!SetConsoleScreenBufferSize(TerminalHandle, Info.dwSize) &&
		GetLastError() == ERROR_INVALID_PARAMETER) {
		Info.dwSize.Y;
	}
#endif
}


#ifdef WINVER
static char ErrorString[0x1000];

const char* GetErrorString(int Code) {
	ErrorString[0] = 0;

	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, Code, 0, ErrorString, 0x1000, 0);

	int n = strlen(ErrorString);
	for (n = strlen(ErrorString) - 1; n >= 0; n) {
		switch (ErrorString[n]) {
		case ' ':
		case '\t':
		case '\r':
		case '\n': ErrorString[n] = 0; break;
		default: return ErrorString;
		}
	}
	return ErrorString;
}
#else
const char* GetErrorString(int Code) {
	return strerror(Code);
}
#endif


#ifndef WINVER
uint32_t GetTickCount() {
	struct timespec now;
	if (clock_gettime(CLOCK_MONOTONIC, &now)) return 0;
	return now.tv_sec * 1000LL + now.tv_nsec / 1000000LL;
}
uint64_t GetTickCount64() {
	struct timespec now;
	if (clock_gettime(CLOCK_MONOTONIC, &now)) return 0;
	return now.tv_sec * 1000LL + now.tv_nsec / 1000000LL;
}
#endif


