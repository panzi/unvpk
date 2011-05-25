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
#ifndef VPK_MAGIC_H
#define VPK_MAGIC_H

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

namespace Vpk {
	class Magic;
	
	typedef boost::shared_ptr<Magic> MagicPtr;
	typedef std::vector<MagicPtr> Magics;
	
	class Magic {
	public:
		Magic(const char *type) : m_type(type) {}
		Magic(const std::string &type) : m_type(type) {}
		virtual ~Magic() {}
	
		const std::string &type() const { return m_type; }
		virtual bool matches(const char magic[], size_t size) const = 0;
		virtual size_t size() const = 0;
	
		static size_t maxSize();
	
		static std::string extensionOf(const std::vector<char> &buf) {
			return extensionOf(&buf[0], buf.size());
		}
	
		static std::string extensionOf(const std::vector<char> &buf, size_t size) {
			return extensionOf(&buf[0], std::min(buf.size(), size));
		}
	
		static std::string extensionOf(const char buf[], size_t size);

		static void add(MagicPtr magic);
		
		static void add(Magic *magic) {
			add(MagicPtr(magic));
		}

		static void remove(MagicPtr magic);

		static void remove(Magic *magic) {
			remove(magic);
		}

	private:
		std::string m_type;
	};
}

#endif
