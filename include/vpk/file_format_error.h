#ifndef VPK_FILE_FORMAT_ERROR_H
#define VPK_FILE_FORMAT_ERROR_H

#include <string>
#include <stdexcept>

namespace Vpk {
	class FileFormatError : public std::runtime_error {
	public:
		FileFormatError() : std::runtime_error("") {}
		FileFormatError(const char *msg) : std::runtime_error(msg) {}
		FileFormatError(const std::string &msg) : std::runtime_error(msg) {}
	};
}

#endif
