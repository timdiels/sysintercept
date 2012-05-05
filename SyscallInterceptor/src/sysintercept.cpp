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
#include <iostream>
#include <IATModifier.h>
#include <common.h>

using namespace std;

struct PipeThreadArgs {
	string pipe_name;
	HANDLE pipe_ready_event;
};

DWORD WINAPI pipe_thread(void*);
HANDLE start_pipe_thread(DWORD);

// Note: win32 closes all handles when the process exits; so you don't have to bother closing handles at process exit.

/*
 * Create child process in suspended state
 * Change its IAT so it'll load our dll
 * Inject a suspended thread that delivers config data to the dll
 * Start the main thread, this will init the process, call DllMain on dlls and eventually call main(),
 * Our DllMain will start and wait for the injected thread
 */
int _tmain(int argc, _TCHAR* argv[]) {
	// TODO if ever adding a help/version message on CLI, include short notice of license
	try {
		const char* exe = "C:\\Windows\\system32\\notepad.exe";
		const _TCHAR* dll = argv[1];

		STARTUPINFO sInfo = {0};
		sInfo.cb = sizeof(STARTUPINFO);
		PROCESS_INFORMATION pInfo;

		cout << "Creating process" << endl;
		throw_if(CreateProcess(exe,  // path to binary to run // TODO once you use lpcommandline, make this NULL
				NULL,  // TODO exe followed by args to exe, beware of correct whitespace and quoting
				NULL,  // security attributes, don't need those
				NULL,  // more security attributes, don't need those
				FALSE, // false = don't inherit handles. Not sure if this'd be needed for stdin, ...
				CREATE_SUSPENDED,
				NULL,  // null = inherit environment
				NULL,  // null = inherit current directory
				&sInfo,
				&pInfo) // info about created process
			== FALSE, "create child process");

		HANDLE pipe_ready_event = start_pipe_thread(pInfo.dwProcessId);

		Process process(pInfo.dwProcessId);

		// Inject dll into IAT
		cout << "Tinkering with IAT" << endl;
		IATModifier iat_modifier(process);
		iat_modifier.setImageBase(process.getImageBase(pInfo.hThread));
		iat_modifier.writeIAT((string)dll);

		// wait for pipe to be created
		throw_if(WaitForSingleObject(pipe_ready_event, INFINITE) != WAIT_OBJECT_0, "wait for pipe");
		CloseHandle(pipe_ready_event);

		// Start main
		cout << "Starting main thread" << endl;
		throw_if(ResumeThread(pInfo.hThread) == -1, "resume thread");

		// Wait for child to exit
		WaitForSingleObject(pInfo.hProcess, INFINITE);
		DWORD exit_code;
		throw_if(GetExitCodeThread(pInfo.hThread, &exit_code) == FALSE, "get thread exit code");
		cout << "Child process exited, child's main thread exit code: " << exit_code << endl;
	}
	catch (std::exception& e)
	{
		cout << "Main thread crashed: " << e.what() << endl;
	}

	// End all threads and exit
	ExitProcess(0);
}

HANDLE start_pipe_thread(DWORD child_pid) {
	PipeThreadArgs* args = new PipeThreadArgs;

	args->pipe_name = get_pipe_name(child_pid);

	args->pipe_ready_event = CreateEvent(NULL, TRUE, FALSE, NULL);
	throw_if(args->pipe_ready_event == NULL, "create pipe_ready_event");

	throw_if(CreateThread(NULL, 0, pipe_thread, args, 0, NULL) == NULL, "create and start pipe_thread");

	return args->pipe_ready_event;
}

// start pipe server that sends config to first thing that connects (i.e. the injected dll)
DWORD WINAPI pipe_thread(void* data) {
	// Named pipe notes:
	// - pipe must be created before client attempts to open it
	// - client must have opened it before you can write to it

	PipeThreadArgs* args = (PipeThreadArgs*)data;
	HANDLE pipe;
	try {
		// Create named pipe
		cout << args->pipe_name << endl;
		pipe = CreateNamedPipe(args->pipe_name.c_str(),
				PIPE_ACCESS_OUTBOUND,  // send to child only
				PIPE_TYPE_BYTE | PIPE_READMODE_BYTE  // binary data transfer
				| PIPE_WAIT,  // asynchronous read/write, don't block
				1,  // allow only pipe instance to be created
				sizeof(Config),  // output buffer size (prevent buffer resizes by immediately passing size of Config struct)
				0,  // input buffer size
				NMPWAIT_WAIT_FOREVER,  // timeout for blocking operations
				NULL);  // security attributes, don't need
		throw_if(pipe == INVALID_HANDLE_VALUE, "create named pipe");

		// Signal we are ready
		throw_if(SetEvent(args->pipe_ready_event) == FALSE, "set pipe_is_listening event");

		// Wait for client to connect
		cout << "Waiting for pipe client" << endl;
		throw_if(ConnectNamedPipe(pipe, NULL) == 0 && GetLastError() != ERROR_PIPE_CONNECTED, // Note: GetLastError gets error of current thread
				"wait for client to connect to pipe"
		);

		// Send config data
		cout << "Piping data" << endl;
		Config conf;
		strcpy_s(conf.message, 5, "data");
		DWORD bytes_written;
		throw_if(WriteFile(pipe, &conf, sizeof(Config), &bytes_written, NULL) == FALSE, "write config to pipe");
		throw_if_(bytes_written != sizeof(Config), "Failed to send Config entirely");

		// Wait for client to receive data
		cout << "Flushing pipe" << endl;
		throw_if(FlushFileBuffers(pipe) == FALSE, "flush pipe");
	} catch (std::exception& e) {
		cout << "pipe_config thread crashed: " << e.what() << endl;
		ExitProcess(0);
	}

	CloseHandle(pipe);
	delete args;

	cout << "Pipe thread exiting" << endl;
	return 0;
}
