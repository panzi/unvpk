#include <algorithm>
#include <map>
#include <set>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <vpk/io.h>
#include <vpk/package.h>
#include <vpk/file_format_error.h>
#include <vpk/console_handler.h>
#include <vpk/file_data_handler_factory.h>
#include <vpk/checking_data_handler_factory.h>

namespace fs = boost::filesystem;

static std::string tolower(const std::string &s) {
	std::string s2(s);
	std::transform(s2.begin(), s2.end(), s2.begin(), (int (*)(int)) std::tolower);
	return s2;
}

static std::string &tolower(std::string &s) {
	std::transform(s.begin(), s.end(), s.begin(), (int (*)(int)) std::tolower);
	return s;
}

void Vpk::Package::read(const boost::filesystem::path &path) {
	std::string filename(path.filename());
		
	if (filename.size() < 8 || tolower(filename.substr(filename.size()-8)) != "_dir.vpk") {
		std::string msg = "file does not end in \"_dir.vpk\": \"";
		msg += path.string();
		msg += "\"";

		if (m_handler) {
			m_handler->archiveerror(Exception(msg), path.string());
			m_name = filename;
		}
		else {
			throw Exception(msg);
		}
	}
	else {
		m_name = filename.substr(0, filename.size()-8);
	}
	m_srcdir = path.parent_path().string();

	fs::ifstream is;
	is.exceptions(std::ios::failbit | std::ios::badbit);
	is.open(path, std::ios::in | std::ios::binary);
	read(is);
}

void Vpk::Package::read(const std::string &srcdir, const std::string &name, std::istream &is) {
	m_srcdir = srcdir;
	m_name   = name;
	read(is);
}

void Vpk::Package::read(std::istream &is) {
	if (readLU32(is) != 0x55AA1234) {
		is.seekg(-4, std::ios_base::cur);
	}
	else {
		unsigned int version   = readLU32(is);
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
		const std::string &type = it->name();
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
			std::string dir = id->name() + "/";

			const Vpk::Files &files = id->files();
			for (Vpk::Files::const_iterator i = files.begin(); i != files.end(); ++ i) {
				os << dir << i->name << "." << type << std::endl;
			}
		}
	}
}

void Vpk::Package::filter(const std::vector<std::string> &paths) {
	std::set<std::string> filefilter;
	std::set<std::string> dirfilter;

	for (std::vector<std::string>::const_iterator i = paths.begin(); i != paths.end(); ++ i) {
		const std::string &name = *i;
		if (!name.empty() && name[i->size()-1] == '/') {
			dirfilter.insert(name);
		}
		else {
			filefilter.insert(name);
			dirfilter.insert(fs::path(name).parent_path().string() + "/");
			dirfilter.insert(name + "/");
		}
	}

	for (Vpk::Types::iterator it = m_types.begin(); it != m_types.end();) {
		const std::string &type = it->name();
		Vpk::Dirs &dirs = it->dirs();
		for (Vpk::Dirs::iterator id = dirs.begin(); id != dirs.end();) {
			std::string dir = id->name();
			dir += "/";

			Vpk::Files &files = id->files();
			if (dirfilter.find(dir) == dirfilter.end()) {
				for (Vpk::Files::iterator i = files.begin(); i != files.end();) {
					std::string filename = dir;
					filename += i->name;
					filename += ".";
					filename += type;
					if (filefilter.find(filename) == filefilter.end()) {
						i = files.erase(i);
					}
					else {
						++ i;
					}
				}
			}

			if (files.empty()) {
				id = dirs.erase(id);
			}
			else {
				++ id;
			}
		}

		if (dirs.empty()) {
			it = m_types.erase(it);
		}
		else {
			++ it;
		}
	}
}

void Vpk::Package::error(const std::string &msg, const std::string &path, ErrorMethod handler) const {
	error(Exception(msg + ": \"" + path + "\""), path, handler);
}

void Vpk::Package::error(const std::exception &exc, const std::string &path, ErrorMethod handler) const {
	if (m_handler) {
		(m_handler->*handler)(exc, path);
	}
	else {
		throw exc;
	}
}

void Vpk::Package::extract(const std::string &destdir, bool check) const {
	FileDataHandlerFactory factory(destdir, check);
	process(factory);
}

void Vpk::Package::check() const {
	CheckingDataHandlerFactory factory;
	process(factory);
}

void Vpk::Package::process(DataHandlerFactory &factory) const {
	std::map<std::string, boost::shared_ptr<fs::ifstream> > archives;

	if (m_handler) m_handler->begin(*this);

	for (Vpk::Types::const_iterator it = m_types.begin(); it != m_types.end(); ++ it) {
		const std::string &type = it->name();
		const Vpk::Dirs &dirs = it->dirs();
		for (Vpk::Dirs::const_iterator id = dirs.begin(); id != dirs.end(); ++ id) {
			fs::path dir(id->name());
			const Vpk::Files &files = id->files();
			for (Vpk::Files::const_iterator i = files.begin(); i != files.end(); ++ i) {
				const Vpk::File &file = *i;
				fs::path filepath(dir / (file.name + "." + type));
				
				if (m_handler) m_handler->extract(filepath.string());
				boost::scoped_ptr<DataHandler> handler;
				
				try {
					handler.reset(factory.create(filepath, file.crc32));
				}
				catch (const std::exception &exc) {
					fileerror(exc, filepath);
					continue;
				}
	
				if (file.size == 0) {
					try {
						handler->process((char*) &file.data[0], file.data.size());
						handler->finish();
					}
					catch (const std::exception &exc) {
						fileerror(exc, filepath);
						continue;
					}
				}
				else {
					std::string archiveName = (boost::format("%s_%03d.vpk") % m_name % file.index).str();
					fs::path archivePath(m_srcdir);
					archivePath /= archiveName;
					boost::shared_ptr<fs::ifstream> archive;
						
					if (archives.find(archiveName) != archives.end()) {
						archive = archives[archiveName];
						if (!archive) {
							continue;
						}
					}
					else {
						if (!fs::exists(archivePath)) {
							archiveerror("archive does not exist", archivePath);

							archives[archiveName] = boost::shared_ptr<fs::ifstream>();
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
					bool fail = false;
					while (left > 0) {
						size_t count = std::min(left, (size_t)BUFSIZ);
						try {
							archive->read(data, count);
						}
						catch (const std::exception& exc) {
							archiveerror(exc, archivePath);
							fail = true;
							break;
						}

						try {
							handler->process(data, count);
						}
						catch (const std::exception& exc) {
							fileerror(exc, filepath);
							fail = true;
							break;
						}

						left -= count;
					}

					if (fail) continue;
					
					try {
						handler->finish();
					}
					catch (const std::exception& exc) {
						fileerror(exc, filepath);
						continue;
					}
					
					if (!file.data.empty()) {
						std::string smallpath = filepath.string() + ".smalldata";

						try {
							handler.reset(factory.create(smallpath, file.crc32));
							handler->process((char*) &file.data[0], file.data.size());
							handler->finish();
						}
						catch (const std::exception& exc) {
							fileerror(exc, smallpath);
							continue;
						}
					}

					if (m_handler) m_handler->success(filepath.string());
				}
			}
		}
	}

	if (m_handler) m_handler->end();
}
