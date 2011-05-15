#ifndef VPK_FILE_H
#define VPK_FILE_H

#include <stdint.h>

#include <iostream>
#include <string>
#include <vector>

namespace Vpk {
	class File {
	public:
		File(const std::string &name,
		     uint32_t crc32 = 0,
			 uint32_t size = 0,
			 uint32_t offset = 0,
			 uint16_t index = 0) :
			name(name),
			crc32(crc32),
			size(size),
			offset(offset),
			index(index),
			data() {}

		void read(std::istream &is);

		std::string name;
		uint32_t    crc32;
		uint32_t    size;
		uint32_t    offset;
		uint16_t    index;
		std::vector<char> data;
	};

	typedef std::vector<File> Files;
}

#endif
