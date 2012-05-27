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
#include "ipc.h" // TODO wow, this is so not confusing

using namespace std;
using namespace boost::interprocess;

// TODO rm rapidxml if no longer using it
//#include "rapidxml/rapidxml.hpp"
//#include "rapidxml/rapidxml_print.hpp" // for testing
//using namespace rapidxml;

wchar_t* get_xml_config(DWORD process_id) {
	shared_memory_object share(open_only, get_ipc_name(process_id).c_str(), read_only);
	mapped_region region(share, read_only); // Note: region is unmapped at dtor
	cout << "yay" << endl;
	wcout << ((wchar_t*)region.get_address()) << endl;

	// TODO For now, just always validate xml input by xsd

	// TODO CodeSynthesis  http://www.codesynthesis.com/products/xsd/
	// doc: http://www.codesynthesis.com/projects/xsd/documentation/cxx/tree/guide/
	//
	// use its tree, before compile clean src/xml/, then xsd cxx-tree --char-type wchar_t --output-dir src/xml --namespace-map http://our/namespace=sysintercept::config --hxx-suffix h --cxx-suffix cpp sysintercept-config.xsd, add --generate-doxygen for free documentation based on the inline annotations in the xsd, to use doxygen: doxygen -g sysintercept.doxygen, to generate doxygen config, then further you can gen doxygen with: doxygen sysintercept.doxygen
	// in using your creation thing: xml_schema::properties props; props.schema_location("http://our/namespace", "our.xsd"); Config* config_ = config("xml string", 0, props));
	// error handling: catch (const xml_schema::exception& e) cout << e;
	//
	// Next: download source/binaries for xsd and compile

	// We assume the XML is valid according to our xsd past this point

	// Note: had a look at Boost.Serilization: easy to use but doesn't support attributes as part of input
	// TODO parse

	return NULL;
}

/*xml_document<wchar_t> document;
document.parse<parse_trim_whitespace>(xml_config);
xml_node* node = document.first_node(L"sysintercept-config")->first_node(L"filesystem")->first_node(L"paths")->first_node();
// TODO loop for multiple
assert(node->name() == (wstring)L"substitution");
xml_node* match = node->first_node(L"match");
xml_node* replacement = node->first_node(L"replacement");
Substition(match->value(), match->first_node(L"format")->value(), replacement->value(), replacement->first_node(L"format")->value());*/
