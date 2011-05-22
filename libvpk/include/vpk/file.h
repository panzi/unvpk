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
#ifndef VPK_FILE_H
#define VPK_FILE_H

#include <stdint.h>

#include <string>
#include <vector>

#include <vpk/node.h>
#include <vpk/file_io.h>

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
			data(0, 0) {}

		Type type() const { return Node::FILE; }
		void read(FileIO &io);

		uint32_t crc32;
		uint32_t size;
		uint32_t offset;
		uint16_t index;
		std::vector<char> data;
	};

	typedef boost::shared_ptr<File> FilePtr;
}

#endif
