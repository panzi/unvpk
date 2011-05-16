#ifndef VPK_HANDLER_H
#define VPK_HANDLER_H

#include <exception>

namespace Vpk {
	class Package;

	class Handler {
	public:
		virtual ~Handler() {}

		virtual void begin(const Package &package) = 0;
		virtual void end() = 0;

		virtual bool direrror(const std::exception &exc, const std::string &path) = 0;
		virtual bool fileerror(const std::exception &exc, const std::string &path) = 0;
		virtual bool archiveerror(const std::exception &exc, const std::string &path) = 0;
		virtual void extract(const std::string &filepath) = 0;
		virtual void success(const std::string &filepath) = 0;
	};
}

#endif
