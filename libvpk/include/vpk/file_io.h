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
#ifndef VPK_FILE_IO_H
#define VPK_FILE_IO_H

#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include <string>

#include <boost/filesystem/operations.hpp>

namespace Vpk {
	// a simple FileIO based on C's FILE* functions
	// this is used because errno+strerror return much more meaningful
	// error messages than iostreams
	//
	// all file operations may throw Vpk::IOError and except open
	// also Vpk::FileReaderClosedError
	class FileIO {
	public:
		enum Whence {
			SET = SEEK_SET,
			END = SEEK_END,
			CUR = SEEK_CUR
		};

		FileIO() : m_stream(0) {}
		FileIO(int fd, const char *mode = "rb") : m_stream(0) { open(fd, mode); }
		FileIO(FILE *stream) : m_stream(stream) {}
		FileIO(const char *filename, const char *mode = "rb") : m_stream(0) { open(filename, mode); }
		FileIO(const std::string &filename, const char *mode = "rb") : m_stream(0) { open(filename, mode); }
		FileIO(const boost::filesystem::path &path, const char *mode = "rb") : m_stream(0) { open(path, mode); }
		FileIO(int fd, const std::string &mode) : m_stream(0) { open(fd, mode); }
		FileIO(const char *filename, const std::string &mode) : m_stream(0) { open(filename, mode); }
		FileIO(const std::string &filename, const std::string &mode) : m_stream(0) { open(filename, mode); }
		FileIO(const boost::filesystem::path &path, const std::string &mode) : m_stream(0) { open(path, mode); }
		FileIO(const FileIO &reader) : m_stream(0) { *this = reader; }
		~FileIO() { if (m_stream) { close(); } }

		FileIO &operator = (const FileIO &reader);

		void clearerr();
		bool error();
		bool eof();
		bool opened() const { return m_stream != 0; }
		bool closed() const { return m_stream == 0; }

		void open(int fd, const char *mode = "rb");
		void open(FILE *stream) { close(); m_stream = stream; }
		void open(const char *filename, const char *mode = "rb");
		void open(const std::string &filename, const char *mode = "rb") { open(filename.c_str(), mode); }
		void open(const boost::filesystem::path &path, const char *mode = "rb") { open(path.string(), mode); }
		void open(int fd, const std::string &mode) { open(fd, mode.c_str()); }
		void open(const char *filename, const std::string &mode) { open(filename, mode.c_str()); }
		void open(const std::string &filename, const std::string &mode) { open(filename.c_str(), mode.c_str()); }
		void open(const boost::filesystem::path &path, const std::string &mode) { open(path.string(), mode.c_str()); }
		void close();

		void setnobuf();

		FILE *stream() { return m_stream; }
		int fileno();

		void seek(off_t offset, Whence whence);
		void seek(off_t offset) { seek(offset, CUR); }
		void rewind();
		off_t tell();
		size_t size() const;
		bool seekable() const;

		void flush();
		void sync();

		void put(int ch);
		int  get();

		void     write(const std::string &str) { write(str.c_str()); }
		void     write(const char *str) { write(str, strlen(str)); }
		void     write(const char *buf, size_t size);

		void     writeAsciiZ(const std::string &str) { writeAsciiZ(str.c_str()); }
		void     writeAsciiZ(const char *str) { write(str, strlen(str)+1); }
		
		void     writeU8(uint8_t    value) { put(value); }
		void     writeS8(int8_t     value) { put(value); }
		void     writeLU16(uint16_t value);
		void     writeLS16(int16_t  value) { writeLU16(value); }
		void     writeLU32(uint32_t value);
		void     writeLS32(int32_t  value) { writeLU32(value); }
		void     writeLU64(uint64_t value);
		void     writeLS64(int64_t  value) { writeLU64(value); }
		void     writeBU16(uint16_t value);
		void     writeBS16(int16_t  value) { writeBU16(value); }
		void     writeBU32(uint32_t value);
		void     writeBS32(int32_t  value) { writeBU32(value); }
		void     writeBU64(uint64_t value);
		void     writeBS64(int64_t  value) { writeBU64(value); }

		// may read less than size if EOF
		size_t   readSome(char *buf, size_t size);
		size_t   readSome(FileIO &dest, size_t size);

		// throws Vpk::IOError when EOF before size is read
		void     read(char *buf, size_t size);
		void     read(FileIO &dest, size_t size);

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
