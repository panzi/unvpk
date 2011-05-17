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
#include <endian.h>
#include <vpk/io.h>

uint8_t Vpk::readU8(std::istream &is) {
	uint8_t value = 0;
	is.read((char*) &value, 1);
	return value;
}

int8_t Vpk::readS8(std::istream &is) {
	int8_t value = 0;
	is.read((char*) &value, 1);
	return value;
}

uint16_t Vpk::readLU16(std::istream &is) {
	uint16_t value = 0;
	is.read((char*) &value, 2);
	return le16toh(value);
}

int16_t Vpk::readLS16(std::istream &is) {
	uint16_t value = 0;
	is.read((char*) &value, 2);
	return (int16_t) le16toh(value);
}

uint32_t Vpk::readLU32(std::istream &is) {
	uint32_t value = 0;
	is.read((char*) &value, 4);
	return le32toh(value);
}

int32_t Vpk::readLS32(std::istream &is) {
	uint32_t value = 0;
	is.read((char*) &value, 4);
	return (int32_t) le32toh(value);
}

uint64_t Vpk::readLU64(std::istream &is) {
	uint64_t value = 0;
	is.read((char*) &value, 8);
	return le64toh(value);
}

int64_t Vpk::readLS64(std::istream &is) {
	uint64_t value = 0;
	is.read((char*) &value, 8);
	return (int64_t) le64toh(value);
}

uint16_t Vpk::readBU16(std::istream &is) {
	uint16_t value = 0;
	is.read((char*) &value, 2);
	return be16toh(value);
}

int16_t Vpk::readBS16(std::istream &is) {
	uint16_t value = 0;
	is.read((char*) &value, 2);
	return (int16_t) be16toh(value);
}

uint32_t Vpk::readBU32(std::istream &is) {
	uint32_t value = 0;
	is.read((char*) &value, 4);
	return be32toh(value);
}

int32_t Vpk::readBS32(std::istream &is) {
	uint32_t value = 0;
	is.read((char*) &value, 4);
	return (int32_t) be32toh(value);
}

uint64_t Vpk::readBU64(std::istream &is) {
	uint64_t value = 0;
	is.read((char*) &value, 8);
	return be64toh(value);
}

int64_t Vpk::readBS64(std::istream &is) {
	uint64_t value = 0;
	is.read((char*) &value, 8);
	return (int64_t) be64toh(value);
}

void Vpk::readAsciiZ(std::istream &is, std::string &s) {
	for (;;) {
		int c = is.get();
		if (c == 0 || !is.good()) return;
		s += (char) c;
	}
}
