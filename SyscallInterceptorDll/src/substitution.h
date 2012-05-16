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

#ifndef SUBSTITUTION_H_
#define SUBSTITUTION_H_

// TODO: apparently boost.regex badly supports unicode. Because of wchar_t... eh?  http://www.boost.org/doc/libs/1_49_0/libs/regex/doc/html/boost_regex/unicode.html
// TODO: there are more possible syntax types http://www.boost.org/doc/libs/1_49_0/libs/regex/doc/html/boost_regex/ref/basic_regex.html

class Substitution {
	public:
		Substitution(std::wstring match, std::wstring match_format, std::wstring replacement, std::wstring replacement_format);

	private:
		boost::wregex match_expression;
		std::wstring replacement;
		boost::match_flag_type replacement_type;
};

#endif
