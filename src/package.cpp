#include <algorithm>
#include <map>
#include <set>

#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <vpk/io.h>
#include <vpk/package.h>
#include <vpk/file_format_error.h>

namespace fs = boost::filesystem;

void create_path(const fs::path &path) {
	fs::path parent = path.parent_path();
	if (!fs::exists(parent)) {
		create_path(parent);
	}
	create_directory(path);
}

void Vpk::Package::read(std::istream &is) {
	if (readLU32(is) != 0x55AA1234) {
		is.seekg(-4, std::ios_base::cur);
	}
	else {
		unsigned int version = readLU32(is);
		unsigned int indexSize = readLU32(is);

		if (version != 1) {
			throw FileFormatError((boost::format("unexpected vpk version %d") % version).str());
		}
	}

	// types
	while (is.good()) {
		std::string name;
		readAsciiZ(is, name);
		if (name.empty()) break;

		m_types.push_back(name);
		m_types.back().read(is);
	}
}

size_t Vpk::Package::filecount() const {
	size_t n = 0;
	
	for (Vpk::Types::const_iterator it = m_types.begin(); it != m_types.end(); ++ it) {
		const Vpk::Dirs &dirs = it->dirs();
		for (Vpk::Dirs::const_iterator id = dirs.begin(); id != dirs.end(); ++ id) {
			n += id->files().size();
		}
	}

	return n;
}

void Vpk::Package::list(std::ostream &os) const {
	for (Vpk::Types::const_iterator it = m_types.begin(); it != m_types.end(); ++ it) {
		const std::string &type = it->name();
		const Vpk::Dirs &dirs = it->dirs();
		for (Vpk::Dirs::const_iterator id = dirs.begin(); id != dirs.end(); ++ id) {
			const std::string &dir = id->name();
			const Vpk::Files &files = id->files();
			for (Vpk::Files::const_iterator i = files.begin(); i != files.end(); ++ i) {
				os << dir << "/" << i->name << "." << type << std::endl;
			}
		}
	}
}

void Vpk::Package::list(const std::vector<std::string> &filter, std::ostream &os) const {
	if (filter.empty()) {
		list(os);
		return;
	}

	std::set<std::string> filefilter;
	std::set<std::string> dirfilter;
	for (std::vector<std::string>::const_iterator i = filter.begin(); i != filter.end(); ++ i) {
		if (!i->empty() && (*i)[i->size()-1] == '/') {
			dirfilter.insert(*i);
		}
		else {
			filefilter.insert(*i);
			dirfilter.insert(*i + "/");
		}
	}

	for (Vpk::Types::const_iterator it = m_types.begin(); it != m_types.end(); ++ it) {
		const std::string &type = it->name();
		const Vpk::Dirs &dirs = it->dirs();
		for (Vpk::Dirs::const_iterator id = dirs.begin(); id != dirs.end(); ++ id) {
			std::string dir = id->name() + "/";
			const Vpk::Files &files = id->files();

			if (dirfilter.find(dir) != dirfilter.end()) {
				for (Vpk::Files::const_iterator i = files.begin(); i != files.end(); ++ i) {
					os << dir << i->name << "." << type << std::endl;
				}
			}
			else {
				for (Vpk::Files::const_iterator i = files.begin(); i != files.end(); ++ i) {
					std::string filename = dir;
					filename += i->name;
					filename += ".";
					filename += type;
					if (filefilter.find(filename) != filefilter.end()) {
						os << filename << std::endl;
					}
				}
			}
		}
	}
}

// TODO: progress as callback
// TODO: proper error handling and option for ignore errors
bool Vpk::Package::extract(const std::string &destdir) const {
	std::map<std::string, boost::shared_ptr<fs::ifstream> > archives;

	for (Vpk::Types::const_iterator it = m_types.begin(); it != m_types.end(); ++ it) {
		const std::string &type = it->name();
		const Vpk::Dirs &dirs = it->dirs();
		for (Vpk::Dirs::const_iterator id = dirs.begin(); id != dirs.end(); ++ id) {
			const Vpk::Files &files = id->files();
			fs::path dir(destdir);
			dir /= id->name();
			
			create_path(dir);
				
			for (Vpk::Files::const_iterator i = files.begin(); i != files.end(); ++ i) {
				const Vpk::File &file = *i;
				fs::path filepath(dir / (file.name + "." + type));
				fs::ofstream os;
				os.exceptions(std::ios::failbit | std::ios::badbit);
				os.open(filepath, std::ios::out | std::ios::binary);
				std::cout << filepath << std::endl;

				if (!os) {
					// TODO error handling
					std::cerr << "error\n";
					return false;
				}
				else if (file.size == 0) {
					os.write((char*) &file.data[0], file.data.size());
					if (os.fail()) {
						// TODO error handling
						return false;
					}
				}
				else {
					std::string archiveName = (boost::format("%s_%03d.vpk") % m_name % file.index).str();
					boost::shared_ptr<fs::ifstream> archive;
					
					if (archives.find(archiveName) != archives.end()) {
						archive = archives[archiveName];

						// TODO error handling
						if (!archive) {
							std::cerr << "no arch\n";
							continue;
						}
					}
					else {
						fs::path archivePath(m_srcdir);
						archivePath /= archiveName;
						if (!fs::exists(archivePath)) {
							// TODO error handling
							archives[archiveName] = archive;
							std::cerr << "no arch (first)\n";
							continue;
						}
						archive.reset(new fs::ifstream);
						archive->exceptions(std::ios::failbit | std::ios::badbit);
						archive->open(archivePath, std::ios::in | std::ios::binary);
						archives[archiveName] = archive;
					}

					archive->seekg(file.offset);
					char data[BUFSIZ];
					size_t left = file.size;
					while (left > 0) {
						size_t block = std::min(left, (size_t)BUFSIZ);
						archive->read(data, block);
						if (archive->fail()) break;
						os.write(data, block);
						left -= block;
					}

					if (os.fail()) {
						// TODO error handling
					}
					
					if (!file.data.empty()) {
						std::string smallpath = filepath.string() + ".smalldata";
						std::ofstream smallos;
						smallos.exceptions(std::ios::failbit | std::ios::badbit);
						smallos.open(smallpath.c_str(), std::ios::out | std::ios::binary);
						smallos.write((char*) &file.data[0], file.data.size());
						smallos.close();
					}
				}

				os.close();
			}
		}
	}

	return true;
}

bool Vpk::Package::extract(const std::vector<std::string> &filter, const std::string &destdir) const {
	if (filter.empty()) {
		return extract(destdir);
	}

	std::set<std::string> filefilter;
	std::set<std::string> dirfilter;
	for (std::vector<std::string>::const_iterator i = filter.begin(); i != filter.end(); ++ i) {
		if (!i->empty() && (*i)[i->size()-1] == '/') {
			dirfilter.insert(*i);
		}
		else {
			filefilter.insert(*i);
			dirfilter.insert(*i + "/");
		}
	}

	// TODO
	std::cerr << "not implemented yet\n";
	return false;
}
