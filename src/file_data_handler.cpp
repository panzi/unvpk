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
#include <vpk/file_data_handler.h>

namespace fs = boost::filesystem;

static void create_path(const fs::path &path) {
	fs::path parent = path.parent_path();
	if (!fs::exists(parent)) {
		create_path(parent);
	}
	create_directory(path);
}

Vpk::FileDataHandler::FileDataHandler(
	const fs::path &path, uint32_t crc32, bool check)
: CheckingDataHandler(path.string(), crc32), m_check(check) {
	create_path(path.parent_path());

	m_os.exceptions(std::ios::failbit | std::ios::badbit);
	m_os.open(path, std::ios::out | std::ios::binary);
}
