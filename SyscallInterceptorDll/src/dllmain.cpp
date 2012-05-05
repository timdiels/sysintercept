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

void ensure_init();

Config conf;

void init_intercept() { // TODO name differently and add note that we have to export at least 1 thing to allow succesful dll load
}

typedef VOID (WINAPI *ExitProcessFPtr)(UINT uExitCode);
ExitProcessFPtr originalExitProcess = NULL;

// "You've been hooked!"
VOID WINAPI ExitProcessHook(UINT uExitCode)
{
	ensure_init();

	MessageBox(0, conf.message, "Good bye!", MB_ICONINFORMATION);

	originalExitProcess(uExitCode);
}

// NOTE: this needs to be in global scope - otherwise the trampolines and hooks
// are deleted when the destructor of nCodeHook is called!
NCodeHookIA32 nCodeHook;
void hookExitProcess()
{
	originalExitProcess = nCodeHook.createHookByName("kernel32.dll", "ExitProcess", ExitProcessHook);
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
		hookExitProcess();
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

