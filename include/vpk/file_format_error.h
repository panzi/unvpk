#ifndef VPK_FILE_FORMAT_ERROR_H
#define VPK_FILE_FORMAT_ERROR_H

#include <vpk/exception.h>

namespace Vpk {
	class FileFormatError : public Exception {
	public:
		FileFormatError() : Exception("") {}
		FileFormatError(const char *msg) : Exception(msg) {}
		FileFormatError(const std::string &msg) : Exception(msg) {}
	};
}

#endif
