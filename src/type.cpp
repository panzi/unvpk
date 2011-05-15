#include <vpk/io.h>
#include <vpk/type.h>

void Vpk::Type::read(std::istream &is) {
	// dirs
	while (is.good()) {
		std::string name;
		readAsciiZ(is, name);
		if (name.empty()) break;

		m_dirs.push_back(name);
		m_dirs.back().read(is);
	}
}
