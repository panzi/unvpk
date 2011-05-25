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
#include <vpk/multipart_magic.h>

void Vpk::MultipartMagic::put(size_t offset, const char magic[], size_t size) {
	m_magics[offset].assign(magic, magic + size);
	if (offset + size > m_size) {
		m_size = offset + size;
	}
}

bool Vpk::MultipartMagic::matches(const char magic[], size_t size) const {
	if (size < m_size) return false;
		
	for (Parts::const_iterator i = m_magics.begin(); i != m_magics.end(); ++ i) {
		if (memcmp(magic + i->first, &i->second[0], i->second.size()) != 0) {
			return false;
		}
	}
	return true;
}
