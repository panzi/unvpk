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
#ifndef VPK_FILE_DATA_HANDLER_H
#define VPK_FILE_DATA_HANDLER_H

#include <boost/crc.hpp>
#include <boost/filesystem/fstream.hpp>

#include <vpk/checking_data_handler.h>

namespace Vpk {
	class FileDataHandler : public CheckingDataHandler {
	public:
		typedef CheckingDataHandler super_type;

		FileDataHandler(const boost::filesystem::path &path, uint32_t crc32, bool check);

		void process(const char *buffer, size_t length) {
			if (m_check) super_type::process(buffer, length);
			m_os.write(buffer, length);
		}

		void finish() {
			m_os.close();
			if (m_check) super_type::finish();
		}

		bool check() const { return m_check; }
	
	private:
		bool                        m_check;
		boost::filesystem::ofstream m_os;
	};
}

#endif
