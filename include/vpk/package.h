#ifndef VPK_PACKAGE_H
#define VPK_PACKAGE_H

#include <iostream>
#include <vector>

#include <vpk/type.h>
#include <vpk/handler.h>

namespace Vpk {
	class Package {
	public:
		Package(const std::string name, const std::string &srcdir) :
			m_name(name), m_srcdir(srcdir), m_types(), m_handler(0) {}

		void read(std::istream &is);

		const std::string &name() const { return m_name; }
		const std::string &srcdir() const { return m_srcdir; }
		const Types &types() const { return m_types; }
		void setHandler(Handler *handler) { m_handler = handler; }
		const Handler *handler() const { return m_handler; }

		void list(std::ostream &os = std::cout) const;
		void list(const std::vector<std::string> &filter, std::ostream &os = std::cout) const;

		bool extract(const std::string &destdir) const;
		bool extract(const std::vector<std::string> &filter, const std::string &destdir) const;

		size_t filecount() const;

	private:
		std::string m_srcdir;
		std::string m_name;
		Types       m_types;
		Handler    *m_handler;
	};
}

#endif
