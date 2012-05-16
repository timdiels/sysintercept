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
#include "interceptconfigshare.h"

using namespace std;
using boost::locale::conv::from_utf;

// Note: win32 closes all handles when the process exits; so you don't have to bother closing handles at process exit.

// TODO list:
// current: and parse xml config into InterceptConfig. boost.serialize for xml is next. allow variable count of file rewrite args to be passed by xml config
// - use rewrite args in file functions
// - add logging for all file related functions we might need for file path rewriting
// - have config contain: verboseness of dll, of cli; path rewrites
// - suggest to ZI list
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
		const wchar_t* xml_config = argv[3]; // Note: can be any unicode XML config doc  TODO DTD or such

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

		//bool cli_verboseness;
		InterceptConfigShare share(xml_config, pInfo.dwProcessId /*, cli_verboseness*/); // Note: share will stay alive for as long as we stay in scope

		//
		Process process(pInfo.dwProcessId);

		// Inject dll into IAT
		LOG(trace) << L"Tinkering with IAT";
		IATModifier iat_modifier(process);
		iat_modifier.setImageBase(process.getImageBase(pInfo.hThread));
		boost::locale::generator gen;
		iat_modifier.writeIAT(from_utf(dll, gen("")));

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

	// TODO cleanup child process on any exit, in case it's still there. Seems it stays alive if we crash before it's started or ended (does boost have a cleanup thing, smart pointers maybe?)
	return 0;
}

