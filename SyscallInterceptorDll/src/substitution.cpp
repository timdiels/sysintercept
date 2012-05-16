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
#include "substitution.h"

#include <boost/regex.hpp>

using namespace std;
using namespace boost;

Substitution::Substitution(wstring match, wstring match_format, wstring replacement, wstring replacement_format) {
	wregex::flag_type type;
	if (match_format == L"perl") {
		type = regex_constants::perl;
	} else if (match_format == L"posix-extended") {
		type = regex_constants::extended;
	} else if (match_format == L"posix-basic") {
		type = regex_constants::basic;
	} else if (match_format == L"literal") {
		type = regex_constants::literal;
	} else {
		assert(false);  // reaching this is total coding error
	}

	match_expression = wregex(match, type);

	if (replacement_format == L"sed") {
		replacement_type = regex_constants::format_sed;
	} else if (replacement_format == L"perl") {
		replacement_type = regex_constants::format_perl;
	} else if (replacement_format == L"literal") {
		replacement_type = regex_constants::format_literal;
	} else {
		assert(false);
	}

	this->replacement = replacement;
}
