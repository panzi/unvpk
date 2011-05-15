#ifndef VPK_DIR_H
#define VPK_DIR_H

#include <iostream>
#include <vector>
#include <string>

#include <vpk/file.h>

namespace Vpk {
	class Dir {
	public:
		Dir(const std::string &name) : m_name(name), m_files() {}

		void read(std::istream &is);

		const std::string &name()  const { return m_name; }
		const Files       &files() const { return m_files; }
		      Files       &files()       { return m_files; }
	
	private:
		std::string m_name;
		Files       m_files;
	};

	typedef std::vector<Dir> Dirs;
}

#endif
