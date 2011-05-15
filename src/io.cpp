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
