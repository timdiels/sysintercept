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
#include <ipc.h>
#include <sysintercept_config.h>
#include "replacement.h"
#include "config.h"
#include "parse.h"

using namespace std;
using namespace boost::interprocess;
namespace xml = sysintercept::config::xml;

auto_ptr<Config> get_xml_config(DWORD process_id) {
	// Get xml string
	shared_memory_object share(open_only, get_ipc_name(process_id).c_str(), read_only);
	mapped_region region(share, read_only); // Note: region is unmapped at dtor
	cout << "yay" << endl;
	wchar_t* xml_config_path = (wchar_t*)region.get_address();
	wchar_t* xsd_path = xml_config_path + wcslen(xml_config_path) + 1;

	// Parse and validate xml
	xml_schema::properties properties;
	properties.schema_location(L"https://github.com/limyreth/sysintercept", xsd_path);
	auto_ptr<xml::Config> xml_config(xml::sysintercept_config(xml_config_path, 0, properties));

	auto_ptr<Config> config(xml_config);
	return config;
}

// TODO put our things in a namespace too? We probably should... right? Or is that only the deal when your code is included in other projects (like that of libs)?

/*xml_document<wchar_t> document;
document.parse<parse_trim_whitespace>(xml_config);
xml_node* node = document.first_node(L"sysintercept-config")->first_node(L"filesystem")->first_node(L"paths")->first_node();
// TODO loop for multiple
assert(node->name() == (wstring)L"substitution");
xml_node* match = node->first_node(L"match");
xml_node* replacement = node->first_node(L"replacement");
Substition(match->value(), match->first_node(L"format")->value(), replacement->value(), replacement->first_node(L"format")->value());*/
