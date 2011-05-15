#ifndef VPK_EXCEPTION_H
#define VPK_EXCEPTION_H

#include <string>
#include <stdexcept>

namespace Vpk {
	class Exception : public std::runtime_error {
	public:
		Exception() : std::runtime_error("") {}
		Exception(const char *msg) : std::runtime_error(msg) {}
		Exception(const std::string &msg) : std::runtime_error(msg) {}
	};
}

#endif
