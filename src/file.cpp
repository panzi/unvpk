#include <algorithm>

#include <vpk/io.h>
#include <vpk/file.h>
#include <vpk/file_format_error.h>

void Vpk::File::read(std::istream &is) {
	crc32 = readLU32(is);
	unsigned int length = readLU16(is);
	index = readLU16(is);
	offset = readLU32(is);
	size = readLU32(is);

	unsigned int terminator = readLU16(is);
	if (terminator != 0xFFFF) {
		throw FileFormatError("invalid terminator");
	}

	if (length > 0) {
		data.resize(length, 0);
		is.read(&data[0], length);
	}
}
