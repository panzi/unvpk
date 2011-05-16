#include <vpk/io.h>
#include <vpk/dir.h>
#include <vpk/file.h>

void Vpk::Dir::read(std::istream &is, const std::string &type) {
	// files
	while (is.good()) {
		std::string name;
		readAsciiZ(is, name);
		if (name.empty()) break;

		name += ".";
		name += type;
		File *file = new File(name);
		m_nodes[name] = NodePtr(file);
		file->read(is);
	}
}
