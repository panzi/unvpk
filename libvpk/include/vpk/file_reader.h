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
#ifndef VPK_FILE_READER_H
#define VPK_FILE_READER_H

#include <stdint.h>
#include <stdio.h>

#include <string>

#include <boost/filesystem/operations.hpp>

namespace Vpk {
	enum Whence {
		SET = SEEK_SET,
		END = SEEK_END,
		CUR = SEEK_CUR
	};

	// a simple binary-only FileReader based on C's FILE* functions
	// this is used because errno+strerror return much more meaningful
	// error messages than iostreams
	//
	// all file operations may throw Vpk::IOError and except open
	// also Vpk::FileReaderClosedError
	class FileReader {
	public:
		FileReader() : m_stream(0) {}
		FileReader(int fd) : m_stream(0) { open(fd); }
		FileReader(FILE *stream) : m_stream(stream) {}
		FileReader(const char *filename) : m_stream(0) { open(filename); }
		FileReader(const std::string &filename) : m_stream(0) { open(filename); }
		FileReader(const boost::filesystem::path &path) : m_stream(0) { open(path); }
		FileReader(const FileReader &reader) : m_stream(0) { *this = reader; }
		~FileReader() { if (m_stream) { close(); } }

		FileReader &operator = (const FileReader &reader);

		void clearerr();
		bool error();
		bool eof();
		bool opened() const { return m_stream != 0; }
		bool closed() const { return m_stream == 0; }

		void open(int fd);
		void open(FILE *stream) { close(); m_stream = stream; }
		void open(const char *filename);
		void open(const std::string &filename) { open(filename.c_str()); }
		void open(const boost::filesystem::path &path) { open(path.string()); }
		void close();

		void setnobuf();

		FILE *stream() { return m_stream; }
		int fileno();

		void seek(long offset, Whence whence);
		void seek(long offset) { seek(offset, CUR); }
		void rewind();
		long tell();

		int get();

		// may read less than size if EOF
		size_t   readSome(char *buf, size_t size);

		// throws Vpk::IOError when EOF before size is read
		void     read(char *buf, size_t size);

		uint8_t  readU8();
		int8_t   readS8();
		uint16_t readLU16();
		int16_t  readLS16();
		uint32_t readLU32();
		int32_t  readLS32();
		uint64_t readLU64();
		int64_t  readLS64();
		uint16_t readBU16();
		int16_t  readBS16();
		uint32_t readBU32();
		int32_t  readBS32();
		uint64_t readBU64();
		int64_t  readBS64();

		void readAsciiZ(std::string &s);
		std::string readAsciiZ() {
			std::string s;
			readAsciiZ(s);
			return s;
		}

	private:
		FILE *m_stream;
	};
}

#endif
