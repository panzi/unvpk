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
#ifndef VPK_IO_H
#define VPK_IO_H

#include <stdint.h>
#include <iostream>
#include <string>

namespace Vpk {
	uint8_t  readU8(  std::istream &is);
	int8_t   readS8(  std::istream &is);
	uint16_t readLU16(std::istream &is);
	int16_t  readLS16(std::istream &is);
	uint32_t readLU32(std::istream &is);
	int32_t  readLS32(std::istream &is);
	uint64_t readLU64(std::istream &is);
	int64_t  readLS64(std::istream &is);
	uint16_t readBU16(std::istream &is);
	int16_t  readBS16(std::istream &is);
	uint32_t readBU32(std::istream &is);
	int32_t  readBS32(std::istream &is);
	uint64_t readBU64(std::istream &is);
	int64_t  readBS64(std::istream &is);

	void readAsciiZ(std::istream &is, std::string &s);
}

#endif
