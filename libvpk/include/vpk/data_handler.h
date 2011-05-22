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
#ifndef VPK_DATA_HANDLER_H
#define VPK_DATA_HANDLER_H

#include <stdint.h>

#include <string>

namespace Vpk {
	class DataHandler {
	public:
		DataHandler(const std::string &path, uint32_t crc32) :
			m_path(path), m_crc32(crc32) {}

		virtual ~DataHandler() {}

		virtual void process(const char *buffer, size_t length) = 0;
		virtual void finish() = 0;
	
		const std::string &path()  const { return m_path; }
		      uint32_t     crc32() const { return m_crc32; }

	private:
		std::string m_path;
		uint32_t    m_crc32;
	};
}

#endif
