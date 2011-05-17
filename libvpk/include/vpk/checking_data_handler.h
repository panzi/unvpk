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
#ifndef VPK_CHECKING_DATA_HANDLER_H
#define VPK_CHECKING_DATA_HANDLER_H

#include <boost/crc.hpp>

#include <vpk/data_handler.h>

namespace Vpk {
	class CheckingDataHandler : public DataHandler {
	public:
		CheckingDataHandler(const std::string &path, uint32_t crc32) :
			DataHandler(path, crc32) {}

		void process(const char *buffer, size_t length) {
			m_hash.process_bytes(buffer, length);
		}

		void finish();
	
	private:
		boost::crc_32_type m_hash;
	};
}

#endif
