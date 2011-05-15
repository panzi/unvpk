#include <vpk/io.h>
#include <vpk/dir.h>

void Vpk::Dir::read(std::istream &is) {
	// files
	while (is.good()) {
		std::string name;
		readAsciiZ(is, name);
		if (name.empty()) break;

		m_files.push_back(name);
		m_files.back().read(is);
	}
}
