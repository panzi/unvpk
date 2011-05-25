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
#ifndef VPK_MULTIPART_MAGIC
#define VPK_MULTIPART_MAGIC

#include <string.h>

#include <map>
#include <vector>

#include <vpk/magic.h>

namespace Vpk {
	class MultipartMagic : public Magic {
	private:
		typedef std::map< size_t, std::vector<char> > Parts;

	public:
		MultipartMagic(const char *type) : Magic(type), m_size(0) {}
		MultipartMagic(const std::string &type) : Magic(type), m_size(0) {}
	
		void put(size_t offset, const std::vector<char> &magic) {
			put(offset, &magic[0], magic.size());
		}

		void put(size_t offset, const char magic[]) {
			put(offset, magic, strlen(magic));
		}
	
		void put(size_t offset, const char magic[], size_t size);
		bool matches(const char magic[], size_t size) const;
		size_t size() const { return m_size; }

	private:
		size_t m_size;
		Parts  m_magics;
	};
}

#endif
