#ifndef VPK_PACKAGE_H
#define VPK_PACKAGE_H

#include <iostream>
#include <vector>

#include <boost/filesystem/operations.hpp>

#include <vpk/type.h>
#include <vpk/handler.h>

namespace Vpk {
	class Package {
	public:
		Package() :
			m_name(), m_srcdir("."), m_types(), m_handler(0) {}

		void read(const char *path) { read(boost::filesystem::path(path)); }
		void read(const std::string &path) { read(boost::filesystem::path(path)); }
		void read(const boost::filesystem::path &path);
		void read(const std::string &srcdir, const std::string &name, std::istream &is);

		const std::string &name() const { return m_name; }
		const std::string &srcdir() const { return m_srcdir; }
		const Types &types() const { return m_types; }
		void setHandler(Handler *handler) { m_handler = handler; }
		const Handler *handler() const { return m_handler; }

		void list(std::ostream &os = std::cout) const;
		void list(const std::vector<std::string> &filter, std::ostream &os = std::cout) const;

		bool extract(const std::string &destdir) const;

		size_t filecount() const;

	private:
		typedef void (Handler::*ErrorMethod)(const std::exception &exc, const std::string &path);

		void read(std::istream &is);

		void direrror(const std::exception &exc, const boost::filesystem::path &path)     const { direrror(exc, path.string()); }
		void fileerror(const std::exception &exc, const boost::filesystem::path &path)    const { fileerror(exc, path.string()); }
		void archiveerror(const std::exception &exc, const boost::filesystem::path &path) const { archiveerror(exc, path.string()); }

		void direrror(const std::exception &exc, const std::string &path)     const { error(exc, path, &Handler::direrror); }
		void fileerror(const std::exception &exc, const std::string &path)    const { error(exc, path, &Handler::fileerror); }
		void archiveerror(const std::exception &exc, const std::string &path) const { error(exc, path, &Handler::archiveerror); }

		void direrror(const std::string &msg, const std::string &path)     const { error(msg, path, &Handler::direrror); }
		void fileerror(const std::string &msg, const std::string &path)    const { error(msg, path, &Handler::fileerror); }
		void archiveerror(const std::string &msg, const std::string &path) const { error(msg, path, &Handler::archiveerror); }
		
		void direrror(const std::string &msg, const boost::filesystem::path &path)     const { direrror(msg, path.string()); }
		void fileerror(const std::string &msg, const boost::filesystem::path &path)    const { fileerror(msg, path.string()); }
		void archiveerror(const std::string &msg, const boost::filesystem::path &path) const { archiveerror(msg, path.string()); }

		void error(const std::string &msg, const std::string &path, ErrorMethod handler) const;
		void error(const std::exception &exc, const std::string &path, ErrorMethod handler) const;

		std::string m_name;
		std::string m_srcdir;
		Types       m_types;
		Handler    *m_handler;
	};
}

#endif
