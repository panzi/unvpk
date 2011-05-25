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
#ifndef VPK_SIMPLE_MAGIC_H
#define VPK_SIMPLE_MAGIC_H

#include <string.h>

#include <vpk/magic.h>

namespace Vpk {
	class SimpleMagic : public Magic {
	public:
		SimpleMagic(const char *type, const char *magic)
			: Magic(type), m_magic(magic, magic + strlen(magic)) {}

		SimpleMagic(const char *type, const char *magic, size_t size)
			: Magic(type), m_magic(magic, magic + size) {}

		SimpleMagic(const std::string &type, const char *magic)
			: Magic(type), m_magic(magic, magic + strlen(magic)) {}

		SimpleMagic(const std::string &type, const char *magic, size_t size)
			: Magic(type), m_magic(magic, magic + size) {}

		bool matches(const char magic[], size_t size) const {
			return size >= m_magic.size() && memcmp(magic, &m_magic[0], m_magic.size()) == 0;
		}
	
		size_t size() const { return m_magic.size(); }

	private:
		std::vector<char> m_magic;
	};
}

#endif
