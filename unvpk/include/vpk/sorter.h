/**
 * unvpk - list, check and extract vpk archives
 * Copyright (C) 2011  Mathias Panzenböck <grosser.meister.morti@gmx.net>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef VPK_SORTER_H
#define VPK_SORTER_H

#include <vpk/list_entry.h>

namespace Vpk {
	enum SortKey {
		SORT_ARCH,
		SORT_CRC32,
		SORT_OFF,
		SORT_SIZE,
		SORT_PATH,
		SORT_RARCH,
		SORT_RCRC32,
		SORT_ROFF,
		SORT_RSIZE,
		SORT_RPATH
	};

	typedef std::vector<SortKey> SortKeys;

	class Sorter {
	public:
		Sorter(const SortKeys &keys) : m_keys(keys) {}
	
		bool operator () (const ListEntry &lhs, const ListEntry &rhs) const;
	
	private:
		SortKeys m_keys;
	};
}

#endif
