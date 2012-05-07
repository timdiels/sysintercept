/*
 * Copyright 2012 Tim Diels
 *
 * This file is part of sysintercept.
 *
 * sysintercept is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * sysintercept is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with sysintercept.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include <NCodeHookInstantiation.h>
#include <tlhelp32.h>
#include <common.h>

using namespace std;

// ntdll vs kernel32, see: http://en.wikipedia.org/wiki/Microsoft_Windows_library_files
//
// ntdll functions (only native apps and often kernel32 go directly to ntdll):
// http://www.geoffchappell.com/studies/windows/win32/ntdll/api/index.htm
// kernel32 functions: (or use dllexp to have a look yourself)
// http://www.geoffchappell.com/studies/windows/win32/kernel32/api/index.htm

bool first_arg = true;

// function start
inline void log_fs(wstring function_name) {
	wcout << L"Dll: " << function_name << L"(";
	first_arg = true;
}

// function arg
template <typename T>
inline void log_arg(T t) {
	if (!first_arg) {
		wcout << L", ";
	}
	wcout << t;
	first_arg = false;
}

// function end
inline void log_fe() {
	wcout << L")" << endl;
}

void hook_everything();
void ensure_init();
VOID (WINAPI *realExitProcess)(UINT uExitCode) = NULL;
HFILE (WINAPI *realOpenFile)(LPCSTR lpFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle) = NULL;

Config conf; // what kind of intercepting to do
// TODO get_configuration with inline static conf and call to ensure_init? To prevent accidentally forgetting?
NCodeHookIA32 nCodeHook;  // its destructor unhooks everything, so we'll keep it alive as a global

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	// Note: There are serious limits on what you can do in a DLL entry point:
	//
	// - You may only call kernel32.dll functions that do not load a library.
	//   It isn't documented which functions are safe to call, but LoadLibrary and LoadLibraryEx
	//   are clearly unsafe.
	//
	// - Because DLL notifications are serialized, do not attempt to communicate with other
	//   threads or processes. Deadlocks may occur as a result.
	//
	// See http://msdn.microsoft.com/en-us/library/windows/desktop/ms682583(v=vs.85).aspx

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		// We aren't allowed to communicate with other processes/threads now (and named pipe funcs load a new library)
		// So just place hooks and fetch hooking info later
		hook_everything();
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

VOID WINAPI newExitProcess(UINT uExitCode)
{
	//MessageBox(0, conf.message, "Good bye!", MB_ICONINFORMATION);
	log_fs(L"ExitProcess");
	log_fe();
	realExitProcess(uExitCode);
}

HFILE WINAPI newOpenFile(LPCSTR lpFileName, LPOFSTRUCT lpReOpenBuff, UINT uStyle) {
	log_fs(L"OpenFile");
	log_fe();
	return realOpenFile(lpFileName, lpReOpenBuff, uStyle);
}

HANDLE (WINAPI* realCreateFileA)(LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) = NULL;
HANDLE WINAPI newCreateFileA(LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
	log_fs(L"CreateFileA");
	log_fe();
	return realCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

HANDLE (WINAPI* realCreateFileW)(LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) = NULL;
HANDLE WINAPI newCreateFileW(LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
	log_fs(L"CreateFileW");
	log_arg((wchar_t*)lpFileName);
	log_arg(dwDesiredAccess);
	log_arg(dwShareMode);
	log_arg(lpSecurityAttributes);
	log_arg(dwCreationDisposition);
	log_arg(dwFlagsAndAttributes);
	log_arg(hTemplateFile);
	log_fe();
	return realCreateFileW((char*)L"b.txt", dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

BOOL (WINAPI* realReadFile)(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped) = NULL;
BOOL WINAPI newReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped) {
	log_fs(L"ReadFile");
	log_arg(hFile);
	log_arg(lpBuffer);
	log_arg(nNumberOfBytesToRead);
	log_arg(lpNumberOfBytesRead);
	log_arg(lpOverlapped);
	log_fe();
	return realReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}

// TODO LZOpenFile?

void hook_everything()  // hook everything we might need
{
	realExitProcess = nCodeHook.createHookByName("kernel32.dll", "ExitProcess", newExitProcess);
	realOpenFile = nCodeHook.createHookByName("kernel32.dll", "OpenFile", newOpenFile);
	realCreateFileA = nCodeHook.createHookByName("kernel32.dll", "CreateFileA", newCreateFileA);
	realCreateFileW = nCodeHook.createHookByName("kernel32.dll", "CreateFileW", newCreateFileW);
	realReadFile = nCodeHook.createHookByName("kernel32.dll", "ReadFile", newReadFile);
}

// inits if we hadn't already (do not call during DllMain!)
void ensure_init() {
	static bool initialized = false;
	if (initialized) return;
	initialized = true;  // try only once

	try {
		string pipe_name = get_pipe_name(GetCurrentProcessId());

		HANDLE pipe = CreateFile(pipe_name.c_str(), GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		throw_if(pipe == INVALID_HANDLE_VALUE, "open pipe");
		DWORD bytes_read;
		throw_if(ReadFile(pipe, &conf, sizeof(Config), &bytes_read, NULL) == FALSE, "read config from pipe");
		throw_if(bytes_read != sizeof(Config), "Config sent through pipe is too small, expected more bytes");
		CloseHandle(pipe);
	} catch (exception& e) {
		MessageBox(0, e.what(), "", MB_ICONINFORMATION);
		throw e;
	}
}

__declspec(dllexport) void dummyexport() {}  // a valid dll needs to export at least one thing
