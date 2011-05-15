#ifndef VPK_HANDLER_H
#define VPK_HANDLER_H

#include <exception>

namespace Vpk {
	class Package;

	enum FilterType {
		SKIP,
		SOME,
		ALL
	};

	class Handler {
	public:
		virtual ~Handler() {}

		virtual void begin(const Package &package) = 0;
		virtual void end() = 0;

		virtual void direrror(const std::exception &exc, const std::string &path) = 0;
		virtual void fileerror(const std::exception &exc, const std::string &path) = 0;
		virtual void archiveerror(const std::exception &exc, const std::string &path) = 0;
		virtual FilterType filterdir( const std::string &path) = 0;
		virtual bool       filterfile(const std::string &path) = 0;
		virtual void       extract(const std::string &filepath) = 0;
		virtual void       success(const std::string &filepath) = 0;
	};
}

#endif
