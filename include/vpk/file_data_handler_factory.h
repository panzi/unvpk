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
#ifndef VPK_FILE_DATA_HANDLER_FACTORY_H
#define VPK_FILE_DATA_HANDLER_FACTORY_H

#include <vpk/file_data_handler.h>
#include <vpk/data_handler_factory.h>

namespace Vpk {
	class FileDataHandlerFactory : public DataHandlerFactory {
	public:
		FileDataHandlerFactory(const std::string &destdir, bool check)
		: m_destdir(destdir), m_check(check) {}

		FileDataHandler *create(const boost::filesystem::path &path, uint32_t crc32) {
			return new FileDataHandler(m_destdir / path, crc32, m_check);
		}

		FileDataHandler *create(const std::string &path, uint32_t crc32) {
			return new FileDataHandler(m_destdir / path, crc32, m_check);
		}

		const boost::filesystem::path &destdir() const { return m_destdir; }
		bool check() const { return m_check; }
	
	private:
		boost::filesystem::path m_destdir;
		bool                    m_check;
	};
}

#endif
