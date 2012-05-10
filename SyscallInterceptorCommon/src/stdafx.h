// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// The following macros define the minimum required platform.  The minimum required platform
// is the earliest version of Windows, Internet Explorer etc. that has the necessary features to run
// your application.  The macros work by enabling all features available on platform versions up to and
// including the version specified.

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER                          // Specifies that the minimum required platform is Windows Vista.
#define WINVER 0x0600           // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
#define _WIN32_WINNT 0x0600     // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINDOWS          // Specifies that the minimum required platform is Windows 98.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE                       // Specifies that the minimum required platform is Internet Explorer 7.0.
#define _WIN32_IE 0x0700        // Change this to the appropriate value to target other versions of IE.
#endif

// Other defines
// Always build everything for unicode (though we can still intercept ANSI versions of win32 functions)
#define UNICODE
#define _UNICODE
#define BOOST_LOG_USE_WCHAR_T

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include <stdio.h>
#include <tchar.h>

// often needed headers that hardly ever change (i.e. dependencies)
#include <boost/filesystem.hpp>  // including this before boost/log/ fixes linker errors (it's a work around)
#include <boost/log/common.hpp>
#include <boost/locale.hpp>  // for wchar_t <-> char conversions
#include "rapidxml/rapidxml.hpp"

#include <string>
#include <sstream>
#include <iostream>

#include "logging.h"


