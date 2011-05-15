#ifndef VPK_TYPE_H
#define VPK_TYPE_H

#include <iostream>
#include <vector>
#include <string>

#include <vpk/dir.h>

namespace Vpk {
	class Type {
	public:
		Type(const std::string &name) : m_name(name), m_dirs() {}

		void read(std::istream &is);

		const std::string  &name() const { return m_name; }
		const Dirs         &dirs() const { return m_dirs; }

	private:
		std::string m_name;
		Dirs        m_dirs;
	};

	typedef std::vector<Type> Types;
}

#endif
