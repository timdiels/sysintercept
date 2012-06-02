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
#include "interceptconfigshare.h"
#include <ipc.h>

using namespace std;
using namespace boost::interprocess;

// TODO: the dealloc could also be done with a remove_shared_memory_on_destroy(name) instance. If that ends up making things prettier...
InterceptConfigShare::InterceptConfigShare(wstring xml_config_path, wstring xsd_path, DWORD child_pid) {
	this->child_pid = child_pid;

	const size_t xml_path_size = xml_config_path.length() + 1;  //+1 for nul-char
	const size_t xsd_path_size = xsd_path.length() + 1;

	shared_memory_object share(create_only, get_ipc_name(child_pid).c_str(), read_write);
	share.truncate(sizeof(wchar_t) * (xml_path_size + xsd_path_size));

	mapped_region region(share, read_write);
	wcsncpy((wchar_t*)region.get_address(), xml_config_path.c_str(), xml_path_size);
	wcsncpy((wchar_t*)region.get_address() + xml_path_size, xsd_path.c_str(), xsd_path_size);
}

InterceptConfigShare::~InterceptConfigShare() {
	shared_memory_object::remove(get_ipc_name(child_pid).c_str());
}
