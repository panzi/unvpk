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
