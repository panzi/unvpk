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
#include <algorithm>

#include <vpk/file.h>
#include <vpk/file_format_error.h>

void Vpk::File::read(FileIO &io) {
	crc32 = io.readLU32();
	unsigned int length = io.readLU16();
	index = io.readLU16();
	offset = io.readLU32();
	size = io.readLU32();

	unsigned int terminator = io.readLU16();
	if (terminator != 0xFFFF) {
		throw FileFormatError("invalid terminator");
	}

	if (length > 0) {
		preload.resize(length, 0);
		io.read(&preload[0], length);
	}
}
