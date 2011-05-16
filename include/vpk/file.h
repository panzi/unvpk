#ifndef VPK_FILE_H
#define VPK_FILE_H

#include <stdint.h>

#include <iostream>
#include <string>
#include <vector>

#include <vpk/node.h>

namespace Vpk {
	class File : public Node {
	public:
		File(const std::string &name,
		     uint32_t crc32 = 0,
			 uint32_t size = 0,
			 uint32_t offset = 0,
			 uint16_t index = 0) :
			Node(name),
			crc32(crc32),
			size(size),
			offset(offset),
			index(index),
			data() {}

		Type type() const { return Node::FILE; }
		void read(std::istream &is);

		uint32_t crc32;
		uint32_t size;
		uint32_t offset;
		uint16_t index;
		std::vector<char> data;
	};

	typedef boost::shared_ptr<File> FilePtr;
}

#endif
