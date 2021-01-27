/**
 * unvpk - list, check and extract vpk archives
 * Copyright (C) 2011  Mathias Panzenböck <grosser.meister.morti@gmx.net>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <map>
#include <set>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>

#include <vpk/util.h>
#include <vpk/dir.h>
#include <vpk/file.h>
#include <vpk/package.h>
#include <vpk/file_format_error.h>
#include <vpk/file_data_handler_factory.h>
#include <vpk/checking_data_handler_factory.h>

namespace fs   = boost::filesystem;
namespace algo = boost::algorithm;

void Vpk::Package::read(const fs::path &path) {
	FileIO io(path, "rb");
	read(path, io);
}

void Vpk::Package::read(const fs::path &path, FileIO &io) {
	fs::path abspath = fs::system_complete(path);
	m_dirfile = abspath.filename().string();
		
	if (m_dirfile.size() < 8 || tolower(m_dirfile.substr(m_dirfile.size()-8)) != "_dir.vpk") {
		Exception exc((boost::format("file does not end in \"_dir.vpk\": \"%s\"")
			% abspath.string()).str());
		if (archiveerror(exc, abspath.string())) {
			throw exc;
		}
		setName(m_dirfile);
	}
	else {
		setName(m_dirfile.substr(0, m_dirfile.size()-8));
	}
	m_srcdir = abspath.parent_path().string();
	
	read(io);
}

void Vpk::Package::read(FileIO &io) {
	size_t headerSize = 0;
	unsigned int indexSize = 0;

	m_version      = 0;
	m_dataOffset   = 0;
	m_footerOffset = 0;
	m_footerSize   = 0;

	if (io.readLU32() != 0x55AA1234) {
		io.seek(-4, FileIO::CUR);
	}
	else {
		m_version = io.readLU32();
		indexSize = io.readLU32();

		if (m_version == 2) {
			m_footerOffset = io.readLU32();
			io.readLU32(); // UNKNOWN
			m_footerSize = io.readLU32();
			io.readLU32(); // UNKNOWN
		}
		else if (m_version != 1) {
			throw FileFormatError((boost::format("unsupported VPK version: %u")
				% m_version).str());
		}

		headerSize   = io.tell();
		m_dataOffset = indexSize + headerSize; // + footerSize?
	}

	std::vector<File*> dirfiles;

	// types
	for (;;) {
		std::string type;
		io.readAsciiZ(type);
		if (type.empty()) break;
		
		// dirs
		for (;;) {
			std::string path;
			io.readAsciiZ(path);
			if (path.empty()) break;

			mkpath(path).read(io, path, type, dirfiles);
		}
	}

	if (m_version == 0) {
		m_dataOffset = io.tell();
	}
	else if (m_version == 1 && io.tell() != m_dataOffset) {
		Exception exc(
			(boost::format("missmatch between header index size (%u) and real index size (%u)")
			% indexSize % (io.tell() - headerSize)).str());
		if (archiveerror(exc, (fs::path(m_srcdir) / (name() + "_dir.vpk")).string())) {
			throw exc;
		}
	}

	for (std::vector<File*>::iterator i = dirfiles.begin(); i != dirfiles.end(); ++ i) {
		(*i)->offset += m_dataOffset;
	}
}

Vpk::Dir &Vpk::Package::mkpath(const char *path) {
	if (!*path) {
		throw Exception("empty path");
	}

	Dir *dir = this;
	const char *ptr = path;
	while (*ptr == '/') ++ ptr;
	while (*ptr) {
		const char *slash = strchr(ptr, '/');
		if (!slash) {
			Node *node = dir->node(ptr);
			if (!node) {
				Dir *newdir = new Dir(ptr);
				dir->add(newdir);
				dir = newdir;
			}
			else if (node->type() != Node::DIR) {
				throw Exception((boost::format("path is not a directory: \"%s\"")
					% path).str());
			}
			else {
				dir = (Dir*) node;
			}
			break;
		}
		else {
			std::string name(ptr, slash - ptr);
			Node *node = dir->node(name);
			if (!node) {
				Dir *newdir = new Dir(name);
				dir->add(newdir);
				dir = newdir;
			}
			else if (node->type() != Node::DIR) {
				throw Exception((boost::format("path is not a directory: \"%s\"")
					% std::string(path, slash - path)).str());
			}
			else {
				dir = (Dir*) node;
			}
			ptr = slash + 1;
			while (*ptr == '/') ++ ptr;
		}
	}

	return *dir;
}

Vpk::Node *Vpk::Package::get(const char *path) {
	if (!*path) return 0;
	
	Node *node = this;
	Dir  *dir  = this;
	const char *ptr = path;
	while (*ptr == '/') ++ ptr;
	while (*ptr) {
		const char *slash = strchr(ptr, '/');
		if (!slash) {
			return dir->node(ptr);
		}
		else {
			node = dir->node(std::string(ptr, slash - ptr));
			if (!node) {
				return 0;
			}
			ptr = slash + 1;
			while (*ptr == '/') ++ ptr;

			if (*ptr) {
				if (node->type() != Node::DIR) {
					return 0;
				}
				dir = (Dir*) node;
			}
		}
	}

	return node;
}

static size_t filecount(const Vpk::Nodes &nodes) {
	size_t n = 0;
	for (Vpk::Nodes::const_iterator it = nodes.begin(); it != nodes.end(); ++ it) {
		Vpk::Node *node = it->second.get();
		if (node->type() == Vpk::Node::DIR) {
			n += filecount(((const Vpk::Dir*) node)->nodes());
		}
		else {
			++ n;
		}
	}

	return n;
}

size_t Vpk::Package::filecount() const {
	return ::filecount(nodes());
}

void Vpk::Package::filter(Dir &dir, const std::set<Node*> &keep) {
	std::vector<std::string> erase;
	for (Nodes::iterator it = dir.begin(); it != dir.end(); ++ it) {
		Node *node = it->second.get();
		bool kept = keep.find(node) != keep.end();
		if (node->type() == Node::DIR) {
			if (!kept) {
				Dir *subdir = (Dir*) node;
				filter(*subdir, keep);
				if (subdir->empty()) {
					erase.push_back(subdir->name());
				}
			}
		}
		else if (!kept) {
			erase.push_back(node->name());
		}
	}

	for (std::vector<std::string>::const_iterator it = erase.begin(); it != erase.end(); ++ it) {
		dir.remove(*it);
	}
}

void Vpk::Package::filter(const std::vector<std::string> &paths) {
	std::set<Node*> keep;
	for (std::vector<std::string>::const_iterator i = paths.begin(); i != paths.end(); ++ i) {
		Node *node = get(*i);
		if (node == this) {
			return;
		}
		else if (node) {
			keep.insert(node);
		}
		else {
			Exception exc("no such file or directory");
			if (filtererror(exc, *i)) {
				throw exc;
			}
		}
	}

	filter(*this, keep);
}

bool Vpk::Package::error(const std::string &msg, const std::string &path, ErrorMethod handler) const {
	return error(Exception(msg + ": \"" + path + "\""), path, handler);
}

bool Vpk::Package::error(const std::exception &exc, const std::string &path, ErrorMethod handler) const {
	if (m_handler) {
		return (m_handler->*handler)(exc, path);
	}
	else {
		return true;
	}
}

std::string Vpk::Package::archiveName(uint16_t index) const {
	if (index == 0x7fff) {
		return m_dirfile;
	}
	else {
		return (boost::format("%s_%03d.vpk") % name() % index).str();
	}
}

boost::filesystem::path Vpk::Package::archivePath(uint16_t index) const {
	return fs::path(m_srcdir) / archiveName(index);
}

void Vpk::Package::extract(const std::string &destdir, bool check) const {
	FileDataHandlerFactory factory(destdir, check);
	process(factory);
}

void Vpk::Package::check() const {
	CheckingDataHandlerFactory factory;
	process(factory);
}

void Vpk::Package::process(const Nodes &nodes,
                           const std::vector<std::string> &prefix,
                           Archives &archives,
                           DataHandlerFactory &factory) const {
	for (Nodes::const_iterator it = nodes.begin(); it != nodes.end(); ++ it) {
		const Node *node = it->second.get();
		std::vector<std::string> pathvec(prefix);
		pathvec.push_back(node->name());
		if (node->type() == Node::DIR) {
			process(((const Dir*) node)->nodes(), pathvec, archives, factory);
		}
		else {
			const File *file = (const File*) node;
			std::string path = algo::join(pathvec, "/");

			if (m_handler) m_handler->extract(path);
			boost::scoped_ptr<DataHandler> dataHandler;
				
			try {
				dataHandler.reset(factory.create(path, file->crc32));
			}
			catch (const std::exception &exc) {
				if (fileerror(exc, path)) throw;
				continue;
			}
	
			size_t preloadSize = file->preload.size();
			if (preloadSize > 0) {
				try {
					dataHandler->process((char*) &file->preload[0], preloadSize);
				}
				catch (const std::exception &exc) {
					if (fileerror(exc, path)) throw;
					continue;
				}
			}

			boost::shared_ptr<FileIO> archive;
			
			Archives::iterator i = archives.find(file->index);
			if (i != archives.end()) {
				archive = i->second;
				if (!archive) {
					Exception exc("archive does not exist");
					if (archiveerror(exc, this->archivePath(file->index).string())) {
						throw exc;
					}
					continue;
				}
			}
			else {
				fs::path archivePath(this->archivePath(file->index));
				if (!fs::exists(archivePath)) {
					Exception exc("archive does not exist");
					if (archiveerror(exc, archivePath.string())) {
						throw exc;
					}

					archives[file->index] = boost::shared_ptr<FileIO>();
					continue;
				}
				archive.reset(new FileIO(archivePath));
				archives[file->index] = archive;
			}

			archive->seek(file->offset, FileIO::SET);
			char data[BUFSIZ];
			size_t left = file->size;
			bool fail = false;
			while (left > 0) {
				size_t count = std::min(left, (size_t)BUFSIZ);
				try {
					archive->read(data, count);
				}
				catch (const std::exception& exc) {
					if (archiveerror(exc, archivePath(file->index).string())) throw;
					fail = true;
					break;
				}

				try {
					dataHandler->process(data, count);
				}
				catch (const std::exception& exc) {
					if (fileerror(exc, path)) throw;
					fail = true;
					break;
				}

				left -= count;
			}

			if (fail) continue;
				
			try {
				dataHandler->finish();
			}
			catch (const std::exception& exc) {
				if (fileerror(exc, path)) throw;
				continue;
			}
			
			if (m_handler) m_handler->success(path);
		}
	}
}

void Vpk::Package::process(DataHandlerFactory &factory) const {
	Archives archives;

	if (m_handler) m_handler->begin(*this);

	process(nodes(), std::vector<std::string>(), archives, factory);

	if (m_handler) m_handler->end();
}
