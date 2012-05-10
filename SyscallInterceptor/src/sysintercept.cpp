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

#include <stdafx.h>
#include <IATModifier.h>
#include <common.h>
#include <rapidxml/rapidxml_print.hpp> // for testing

// TODO move logging init to other file, along with the tons of namespace and hpps


using namespace std;
using namespace rapidxml;
using boost::locale::conv::from_utf;

struct PipeThreadArgs {
	wstring pipe_name;
	HANDLE pipe_ready_event;
};

DWORD WINAPI pipe_thread(void*);
HANDLE start_pipe_thread(DWORD);

// Note: win32 closes all handles when the process exits; so you don't have to bother closing handles at process exit.

// TODO list:
// - use proper logging library and log to file: using boost-log
// - add logging for all file related functions we might need for file path rewriting
// - allow variable count of file rewrite args
// - use rewrite args in functions
// ------ file path rewrite functionality is done now ------
// - TODO be sure that stdin, stdout and cli args are passed on correctly
// ------ is now usable if you manage to compile it, but no doc or other nifty bits ------
// - add -v verbosity level for omitting some logging, default should mean nothing is logged
// - use ZI to get our dependencies in here rather than just putting a copy in working tree (might first want to ask ZI mailing list on how other C++ devs do that).
// - once we add a -h --help and -V --version message on CLI, optionally include short notice of license (find a pretty print library for standard help message printing)
// - make compiling easier/better (maybe ZI with dependencies nicely separated from the rest) so we can make a first release v1
// - suggest to haskell for prefix fix, on ZI list, ...
// - what about win64 support, testing it works everywhere in any program? ... stability?
// - could tweak boost.log by building it with BOOST_LOG_USE_WCHAT_T, ...

int _tmain(int argc, wchar_t const* argv[]) {
	try {
		init_logging("sysintercept_cli.log");

		const wchar_t* dll = argv[1]; // Note: can be any unicode
		const wchar_t* exe = argv[2]; // Note: unicode string limited to latin1

		// xml arg may contain any unicode

		// rapidxml notes:
		// - xml_document points to argv[3] string, it doesn't copy it. The string must live as long as doc lives.
		// - even though it takes char*, it supports UTF
		/*xml_document<> doc;
		doc.parse<0>((char*)argv[3]);
		cout << doc << endl;
		return 0;*/

		STARTUPINFO sInfo = {0};
		sInfo.cb = sizeof(STARTUPINFO);
		PROCESS_INFORMATION pInfo;

		LOG(trace) << L"Creating process";
		throw_if(CreateProcess(exe,  // path to binary to run // TODO once you use lpcommandline, make this NULL
				NULL,  // exe followed by args to exe  TODO pass argv[4] and so on, beware of correct whitespace and quoting
				NULL,  // security attributes, don't need those
				NULL,  // more security attributes, don't need those
				FALSE, // false = don't inherit handles. Not sure if this'd be needed for stdin, ...
				CREATE_SUSPENDED,
				NULL,  // null = inherit environment
				NULL,  // null = inherit current directory
				&sInfo,
				&pInfo) // info about created process
			== FALSE, L"create child process");

		HANDLE pipe_ready_event = start_pipe_thread(pInfo.dwProcessId);

		Process process(pInfo.dwProcessId);

		// Inject dll into IAT
		LOG(trace) << L"Tinkering with IAT";
		IATModifier iat_modifier(process);
		iat_modifier.setImageBase(process.getImageBase(pInfo.hThread));
		boost::locale::generator gen;
		iat_modifier.writeIAT(from_utf(dll, gen("")));

		// wait for pipe to be created
		throw_if(WaitForSingleObject(pipe_ready_event, INFINITE) != WAIT_OBJECT_0, L"wait for pipe");
		CloseHandle(pipe_ready_event);

		// Start main
		LOG(trace) << L"Starting child process";
		throw_if(ResumeThread(pInfo.hThread) == -1, L"resume thread");

		// Wait for child to exit
		WaitForSingleObject(pInfo.hProcess, INFINITE);
		DWORD exit_code;
		throw_if(GetExitCodeThread(pInfo.hThread, &exit_code) == FALSE, L"get thread exit code");
		LOG(info) << L"Child process exited with code " << exit_code;
	} catch (boost::exception& e) { // TODO: reuse this error handling
		if(wstring const* werr = boost::get_error_info<werror>(e)) {
			LOG(fatal) << L"Main thread crashed: " << *werr;
		} else {
			LOG(fatal) << L"Main thread crashed";
		}
	} catch (std::exception& e) {
			LOG(fatal) << L"Main thread crashed: " << e.what();
	} catch (...) {
		LOG(fatal) << L"Main thread crashed";
	}

	// TODO cleanup child process on any exit, in case it's still there. Seems it stays alive if we crash before it's started or ended
	// End all threads and exit
	ExitProcess(0);
}

HANDLE start_pipe_thread(DWORD child_pid) {
	PipeThreadArgs* args = new PipeThreadArgs;

	args->pipe_name = get_pipe_name(child_pid);

	args->pipe_ready_event = CreateEvent(NULL, TRUE, FALSE, NULL);
	throw_if(args->pipe_ready_event == NULL, L"create pipe_ready_event");

	throw_if(CreateThread(NULL, 0, pipe_thread, args, 0, NULL) == NULL, L"create and start pipe_thread");

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
		pipe = CreateNamedPipe(args->pipe_name.c_str(),
				PIPE_ACCESS_OUTBOUND,  // send to child only
				PIPE_TYPE_BYTE | PIPE_READMODE_BYTE  // binary data transfer
				| PIPE_WAIT,  // asynchronous read/write, don't block
				1,  // allow only pipe instance to be created
				sizeof(Config),  // output buffer size (prevent buffer resizes by immediately passing size of Config struct)
				0,  // input buffer size
				NMPWAIT_WAIT_FOREVER,  // timeout for blocking operations
				NULL);  // security attributes, don't need
		throw_if(pipe == INVALID_HANDLE_VALUE, L"create named pipe");

		// Signal we are ready
		throw_if(SetEvent(args->pipe_ready_event) == FALSE, L"set pipe_is_listening event");

		// Wait for client to connect
		LOG(trace) << L"Waiting for pipe client";
		throw_if(ConnectNamedPipe(pipe, NULL) == 0 && GetLastError() != ERROR_PIPE_CONNECTED, // Note: GetLastError gets error of current thread
				L"wait for client to connect to pipe"
		);

		// Send config data
		LOG(trace) << L"Piping data";
		Config conf;
		strcpy_s(conf.message, 5, "data");
		DWORD bytes_written;
		throw_if(WriteFile(pipe, &conf, sizeof(Config), &bytes_written, NULL) == FALSE, L"write config to pipe");
		throw_if_(bytes_written != sizeof(Config), L"Failed to send Config entirely");

		// Wait for client to receive data
		LOG(trace) << L"Flushing pipe";
		throw_if(FlushFileBuffers(pipe) == FALSE, L"flush pipe");
	} catch (std::exception& e) {
		cerr << "whoops" << endl;
		LOG(fatal) << L"pipe_config thread crashed: " << e.what();
		ExitProcess(0);
	}

	CloseHandle(pipe);
	delete args;

	LOG(trace) << L"Pipe thread exiting";
	return 0;
}
