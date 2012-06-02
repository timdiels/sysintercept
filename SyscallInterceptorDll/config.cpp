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
#include "config.h"

using namespace std;
using namespace boost;

namespace xml = sysintercept::config::xml;

Config::Config(xml::Config& xml_config) {
	xml::Path::replacement_sequence rules = xml_config->filesystem().path().replacement();
	path_replacement_rules.reserve(rules.size());
	BOOST_FOREACH(xml::Replacement& r, rules) {
		path_replacement_rules.push_back(Replacement(r));
	}
}

std::wstring transform_path(const std::wstring& path) {
	wstring retval = path;
	BOOST_FOREACH(Replacement& r, path_replacement_rules) {
		retval = r.apply_to(retval);
	}
	return retval;
}
