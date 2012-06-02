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

#ifndef INTERCEPTCONFIGSHARE_H_
#define INTERCEPTCONFIGSHARE_H_

// Handles allocation/deallocation of InterceptConfig in shared memory
class InterceptConfigShare {
	public:
		InterceptConfigShare(std::wstring xml_config_path, std::wstring xsd_path, DWORD child_pid);
		~InterceptConfigShare();

	private:
		DWORD child_pid;
};

#endif /* INTERCEPTCONFIG_H_ */
