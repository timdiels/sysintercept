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

#include "stdafx.h"
#include <sstream>

struct Config {
	char message[5];
};

template <class T>
inline std::string to_str(const T& t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}

inline void throw_if_(bool throw_, const std::string& what) {
	if (throw_) throw std::runtime_error(what);
}

inline void throw_if(bool throw_, const std::string& what) {
	throw_if_(throw_, "Failed to " + what + ", GetLastError=" + to_str(GetLastError()));
}

inline std::string get_pipe_name(DWORD process_id) {
	return "\\\\.\\pipe\\sysintercept" + to_str(process_id);
}


