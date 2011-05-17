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
#ifndef VPK_CHECKING_DATA_HANDLER_FACTORY_H
#define VPK_CHECKING_DATA_HANDLER_FACTORY_H

#include <vpk/checking_data_handler.h>
#include <vpk/data_handler_factory.h>

namespace Vpk {
	class CheckingDataHandlerFactory : public DataHandlerFactory {
	public:
		CheckingDataHandler *create(const std::string &path, uint32_t crc32) {
			return new CheckingDataHandler(path, crc32);
		}
	};
}

#endif
