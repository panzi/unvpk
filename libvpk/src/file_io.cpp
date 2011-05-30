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

#include <vpk/file_io.h>
#include <vpk/file_io_closed_error.h>
#include <vpk/io_error.h>

#include <endian.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

#if (_POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE) && defined(__linux__)
#include <sys/sendfile.h>
#endif

Vpk::FileIO &Vpk::FileIO::operator = (const FileIO &reader) {
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

void Vpk::FileIO::clearerr() {
	if (!m_stream) throw FileIOClosedError();
	::clearerr(m_stream);
}

bool Vpk::FileIO::error() {
	if (!m_stream) throw FileIOClosedError();
	return ferror(m_stream) != 0;
}

bool Vpk::FileIO::eof() {
	if (!m_stream) throw FileIOClosedError();
	return feof(m_stream) != 0;
}

void Vpk::FileIO::open(int fd, const char *mode) {
#if _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE
	if (m_stream) close();
	m_stream = fdopen(fd, mode);
	if (!m_stream) throw IOError(errno);
#else
	throw IOError(ENOSYS);
#endif
}

void Vpk::FileIO::open(const char *filename, const char *mode) {
	if (m_stream) {
		m_stream = freopen(filename, mode, m_stream);
	}
	else {
		m_stream = fopen(filename, mode);
	}
	if (!m_stream) throw IOError(errno);
}

void Vpk::FileIO::setnobuf() {
	if (!m_stream) throw FileIOClosedError();
	if (setvbuf(m_stream, NULL, _IONBF, 0) != 0) {
		throw IOError(errno);
	}
}

int Vpk::FileIO::fileno() {
#if _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE
	return m_stream == 0 ? -1 : ::fileno(m_stream);
#else
	// or throw IOError(ENOSYS)?
	return -1;
#endif
}

void Vpk::FileIO::rewind() {
	if (!m_stream) throw FileIOClosedError();
	::rewind(m_stream);
}

void Vpk::FileIO::close() {
	if (m_stream) {
		int code = fclose(m_stream);
		m_stream = 0;
		if (code != 0) {
			throw IOError(errno);
		}
	}
}

void Vpk::FileIO::seek(off_t offset, Whence whence) {
	if (!m_stream) throw FileIOClosedError();
	if (fseeko(m_stream, offset, whence) != 0) {
		throw IOError(errno);
	}
}

off_t Vpk::FileIO::tell() {
	if (!m_stream) throw FileIOClosedError();
	off_t pos = ftello(m_stream);
	if (pos == -1) {
		throw IOError(errno);
	}
	return pos;
}

size_t Vpk::FileIO::size() const {
	if (!m_stream) throw FileIOClosedError();
	struct stat stbuf;
	memset(&stbuf, 0, sizeof(stbuf));
	if (fstat(::fileno(m_stream), &stbuf) != 0) {
		throw IOError(errno);
	}
	return stbuf.st_size;
}

bool Vpk::FileIO::seekable() const {
	if (!m_stream) throw FileIOClosedError();
	if (ftello(m_stream) < 0) {
		int errnum = errno;
		if (errnum == EBADF) {
			return false;
		}
		throw IOError(errnum);
	}
	return true;
}

void Vpk::FileIO::flush() {
	if (!m_stream) throw FileIOClosedError();
	if (fflush(m_stream) != 0) {
		throw IOError(errno);
	}
}

void Vpk::FileIO::sync() {
#if _BSD_SOURCE || _XOPEN_SOURCE
	flush();
	if (fsync(::fileno(m_stream)) != 0) {
		throw IOError(errno);
	}
#else
	throw IOError(ENOSYS);
#endif
}

void Vpk::FileIO::put(int ch) {
	if (!m_stream) throw FileIOClosedError();
	if (fputc(ch, m_stream) == EOF) {
		throw IOError(errno);
	}
}

int Vpk::FileIO::get() {
	if (!m_stream) throw FileIOClosedError();
	int ch = fgetc(m_stream);
	if (ch == EOF && ferror(m_stream)) {
		throw IOError(errno);
	}
	return ch;
}

void Vpk::FileIO::write(const char *buf, size_t size) {
	if (!m_stream) throw FileIOClosedError();
	if (fwrite(buf, size, 1, m_stream) < 1) {
		throw IOError(errno);
	}
}

void Vpk::FileIO::writeLU16(uint16_t value) {
	value = htole16(value);
	write((const char*) &value, sizeof(value));
}

void Vpk::FileIO::writeLU32(uint32_t value) {
	value = htole32(value);
	write((const char*) &value, sizeof(value));
}

void Vpk::FileIO::writeLU64(uint64_t value) {
	value = htole64(value);
	write((const char*) &value, sizeof(value));
}

void Vpk::FileIO::writeBU16(uint16_t value) {
	value = htobe16(value);
	write((const char*) &value, sizeof(value));
}

void Vpk::FileIO::writeBU32(uint32_t value) {
	value = htobe32(value);
	write((const char*) &value, sizeof(value));
}

void Vpk::FileIO::writeBU64(uint64_t value) {
	value = htobe64(value);
	write((const char*) &value, sizeof(value));
}

size_t Vpk::FileIO::readSome(char *buf, size_t size) {
	if (!m_stream) throw FileIOClosedError();
	size_t count = fread(buf, 1, size, m_stream);
	if (count < size && ferror(m_stream)) {
		throw IOError(errno);
	}
	return count;
}

size_t Vpk::FileIO::readSome(FileIO &dest, size_t size) {
	if (!m_stream || !dest.m_stream) throw FileIOClosedError();
#if (_POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE) && defined(__linux__)
	off_t pos = ftello(m_stream);
	if (pos >= 0) {
		dest.flush();
		ssize_t count = sendfile(
			::fileno(dest.m_stream),
			::fileno(m_stream), &pos, size);
		if (count < 0) {
			throw IOError(errno);
		}
		if (fseeko(m_stream, pos, SEEK_SET) < 0) {
			throw IOError(errno);
		}
		return count;
	}
	else {
#endif
		char buf[BUFSIZ];
		size_t left = size;
		while (left > 0) {
			size_t chunkSize = std::min(left, (size_t)BUFSIZ);
			size_t count = fread(buf, 1, chunkSize, m_stream);
			if (count < chunkSize) {
				if (ferror(m_stream)) {
					throw IOError(errno);
				}
				else {
					break;
				}
			}
			if (fwrite(buf, count, 1, dest.m_stream) < 1) {
				throw IOError(errno);
			}
			left -= count;
		}
		return size - left;
#if (_POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _POSIX_SOURCE) && defined(__linux__)
	}
#endif
}

void Vpk::FileIO::read(char *buf, size_t size) {
	if (!m_stream) throw FileIOClosedError();
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

void Vpk::FileIO::read(FileIO &dest, size_t size) {
	if (readSome(dest, size) < size) {
		throw IOError(EOF);
	}
}

uint8_t Vpk::FileIO::readU8() {
	uint8_t value = 0;
	read((char*) &value, 1);
	return value;
}

int8_t Vpk::FileIO::readS8() {
	int8_t value = 0;
	read((char*) &value, 1);
	return value;
}

uint16_t Vpk::FileIO::readLU16() {
	uint16_t value = 0;
	read((char*) &value, 2);
	return le16toh(value);
}

int16_t Vpk::FileIO::readLS16() {
	uint16_t value = 0;
	read((char*) &value, 2);
	return (int16_t) le16toh(value);
}

uint32_t Vpk::FileIO::readLU32() {
	uint32_t value = 0;
	read((char*) &value, 4);
	return le32toh(value);
}

int32_t Vpk::FileIO::readLS32() {
	uint32_t value = 0;
	read((char*) &value, 4);
	return (int32_t) le32toh(value);
}

uint64_t Vpk::FileIO::readLU64() {
	uint64_t value = 0;
	read((char*) &value, 8);
	return le64toh(value);
}

int64_t Vpk::FileIO::readLS64() {
	uint64_t value = 0;
	read((char*) &value, 8);
	return (int64_t) le64toh(value);
}

uint16_t Vpk::FileIO::readBU16() {
	uint16_t value = 0;
	read((char*) &value, 2);
	return be16toh(value);
}

int16_t Vpk::FileIO::readBS16() {
	uint16_t value = 0;
	read((char*) &value, 2);
	return (int16_t) be16toh(value);
}

uint32_t Vpk::FileIO::readBU32() {
	uint32_t value = 0;
	read((char*) &value, 4);
	return be32toh(value);
}

int32_t Vpk::FileIO::readBS32() {
	uint32_t value = 0;
	read((char*) &value, 4);
	return (int32_t) be32toh(value);
}

uint64_t Vpk::FileIO::readBU64() {
	uint64_t value = 0;
	read((char*) &value, 8);
	return be64toh(value);
}

int64_t Vpk::FileIO::readBS64() {
	uint64_t value = 0;
	read((char*) &value, 8);
	return (int64_t) be64toh(value);
}

void Vpk::FileIO::readAsciiZ(std::string &s) {
	if (!m_stream) throw FileIOClosedError();
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
