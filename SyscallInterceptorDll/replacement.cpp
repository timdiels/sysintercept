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
#include <sysintercept_config.h>
#include "replacement.h"

using namespace std;
using namespace boost;

namespace xml = sysintercept::config::xml;

Replacement::Replacement(xml::Replacement& r) {
	wregex::flag_type type;
	xml::ReplacementFromFormat from_format = r.from().format();
	if (from_format == xml::ReplacementFromFormat::perl) {
		type = regex_constants::perl;
	} else if (from_format == xml::ReplacementFromFormat::posix_extended) {
		type = regex_constants::extended;
	} else if (from_format == xml::ReplacementFromFormat::posix_basic) {
		type = regex_constants::basic;
	} else if (from_format == xml::ReplacementFromFormat::literal) {
		type = regex_constants::literal;
	} else {
		assert(false);  // reaching this is total coding error
	}

	LOG(debug) << r.from() << " " << type;
	match_expression = wregex(r.from(), type);

	xml::ReplacementToFormat to_format = r.to().format();
	if (to_format == xml::ReplacementToFormat::sed) {
		match_flags = regex_constants::format_sed;
	} else if (to_format == xml::ReplacementToFormat::perl) {
		match_flags = regex_constants::format_perl;
	} else if (to_format == xml::ReplacementToFormat::literal) {
		match_flags = regex_constants::format_literal;
	} else {
		assert(false);
	}

	LOG(debug) << r.to() << " " << match_flags;

	this->replacement = r.to();
}

wstring Replacement::apply_to(const wstring& str) {
	wstring result = regex_replace(str, match_expression, replacement, match_flags);
	LOG(debug) << str << " -> " << result;
	return result;
}



