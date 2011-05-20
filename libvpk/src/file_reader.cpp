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

#include <vpk/file_reader.h>
#include <vpk/file_reader_closed_error.h>
#include <vpk/io_error.h>

#include <endian.h>
#include <errno.h>
#include <unistd.h>

Vpk::FileReader &Vpk::FileReader::operator = (const FileReader &reader) {
	close();
	if (reader.m_stream) {
		int fd = dup(::fileno(reader.m_stream));
		if (fd == -1) {
			throw IOError(errno);
		}
		open(fd);
	}
	return *this;
}

void Vpk::FileReader::clearerr() {
	if (!m_stream) throw FileReaderClosedError();
	::clearerr(m_stream);
}

bool Vpk::FileReader::error() {
	if (!m_stream) throw FileReaderClosedError();
	return ferror(m_stream) != 0;
}

bool Vpk::FileReader::eof() {
	if (!m_stream) throw FileReaderClosedError();
	return feof(m_stream) != 0;
}

void Vpk::FileReader::open(int fd) {
#if _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE
	if (m_stream) close();
	m_stream = fdopen(fd, "rb");
	if (!m_stream) throw IOError(errno);
#else
	throw IOError(ENOSYS);
#endif
}

void Vpk::FileReader::open(const char *filename) {
	if (m_stream) {
		m_stream = freopen(filename, "rb", m_stream);
	}
	else {
		m_stream = fopen(filename, "rb");
	}
	if (!m_stream) throw IOError(errno);
}

void Vpk::FileReader::setnobuf() {
	if (!m_stream) throw FileReaderClosedError();
	if (setvbuf(m_stream, NULL, _IONBF, 0) != 0) {
		throw IOError(errno);
	}
}

int Vpk::FileReader::fileno() {
#if _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE
	return m_stream == 0 ? -1 : ::fileno(m_stream);
#else
	// or throw IOError(ENOSYS)?
	return -1;
#endif
}

void Vpk::FileReader::rewind() {
	if (!m_stream) throw FileReaderClosedError();
	::rewind(m_stream);
}

void Vpk::FileReader::close() {
	if (m_stream) {
		int code = fclose(m_stream);
		m_stream = 0;
		if (code != 0) {
			throw IOError(errno);
		}
	}
}

void Vpk::FileReader::seek(long offset, Whence whence) {
	if (!m_stream) throw FileReaderClosedError();
	if (fseek(m_stream, offset, whence) != 0) {
		throw IOError(errno);
	}
}

long Vpk::FileReader::tell() {
	if (!m_stream) throw FileReaderClosedError();
	long pos = ftell(m_stream);
	if (pos == -1L) {
		throw IOError(errno);
	}
	return pos;
}

int Vpk::FileReader::get() {
	if (!m_stream) throw FileReaderClosedError();
	int ch = fgetc(m_stream);
	if (ch == EOF && ferror(m_stream)) {
		throw IOError(errno);
	}
	return ch;
}

size_t Vpk::FileReader::readSome(char *buf, size_t size) {
	if (!m_stream) throw FileReaderClosedError();
	size_t count = fread(buf, 1, size, m_stream);
	if (count < size && ferror(m_stream)) {
		throw IOError(errno);
	}
	return count;
}

void Vpk::FileReader::read(char *buf, size_t size) {
	if (!m_stream) throw FileReaderClosedError();
	size_t count = fread(buf, size, 1, m_stream);
	if (count < 1) {
		if (ferror(m_stream)) {
			throw IOError(errno);
		}
		else { // feof(m_stream)
			throw IOError(EOF);
		}
	}
}

uint8_t Vpk::FileReader::readU8() {
	uint8_t value = 0;
	read((char*) &value, 1);
	return value;
}

int8_t Vpk::FileReader::readS8() {
	int8_t value = 0;
	read((char*) &value, 1);
	return value;
}

uint16_t Vpk::FileReader::readLU16() {
	uint16_t value = 0;
	read((char*) &value, 2);
	return le16toh(value);
}

int16_t Vpk::FileReader::readLS16() {
	uint16_t value = 0;
	read((char*) &value, 2);
	return (int16_t) le16toh(value);
}

uint32_t Vpk::FileReader::readLU32() {
	uint32_t value = 0;
	read((char*) &value, 4);
	return le32toh(value);
}

int32_t Vpk::FileReader::readLS32() {
	uint32_t value = 0;
	read((char*) &value, 4);
	return (int32_t) le32toh(value);
}

uint64_t Vpk::FileReader::readLU64() {
	uint64_t value = 0;
	read((char*) &value, 8);
	return le64toh(value);
}

int64_t Vpk::FileReader::readLS64() {
	uint64_t value = 0;
	read((char*) &value, 8);
	return (int64_t) le64toh(value);
}

uint16_t Vpk::FileReader::readBU16() {
	uint16_t value = 0;
	read((char*) &value, 2);
	return be16toh(value);
}

int16_t Vpk::FileReader::readBS16() {
	uint16_t value = 0;
	read((char*) &value, 2);
	return (int16_t) be16toh(value);
}

uint32_t Vpk::FileReader::readBU32() {
	uint32_t value = 0;
	read((char*) &value, 4);
	return be32toh(value);
}

int32_t Vpk::FileReader::readBS32() {
	uint32_t value = 0;
	read((char*) &value, 4);
	return (int32_t) be32toh(value);
}

uint64_t Vpk::FileReader::readBU64() {
	uint64_t value = 0;
	read((char*) &value, 8);
	return be64toh(value);
}

int64_t Vpk::FileReader::readBS64() {
	uint64_t value = 0;
	read((char*) &value, 8);
	return (int64_t) be64toh(value);
}

void Vpk::FileReader::readAsciiZ(std::string &s) {
	if (!m_stream) throw FileReaderClosedError();
	for (;;) {
		int ch = fgetc(m_stream);
		if (ch == 0) return;
		if (ch == EOF) {
			if (ferror(m_stream)) {
				throw IOError(errno);
			}
			else { // feof(m_stream)
				throw IOError(EOF);
			}
		}
		s += (char) ch;
	}
}
