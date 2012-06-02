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

#pragma once

// TODO split this file

template <class T>
inline std::wstring to_str(const T& t)
{
	std::wstringstream ss;
	ss << t;
	return ss.str();
}

// exception stuff /////////
#include <boost/exception/all.hpp>
typedef boost::error_info<struct werror_message, std::wstring> werror;
struct wruntime_error: virtual boost::exception, virtual std::exception { };

inline void throw_if_(bool throw_, const std::wstring& what) {
	if (throw_) throw wruntime_error() << werror(what);
}

inline void throw_if(bool throw_, const std::wstring& what) {
	throw_if_(throw_, L"Failed to " + what + L", GetLastError=" + to_str(GetLastError()));
}
////////////////////////////


