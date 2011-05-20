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
#include <algorithm>
#include <map>
#include <set>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>

#include <vpk/dir.h>
#include <vpk/file.h>
#include <vpk/package.h>
#include <vpk/file_format_error.h>
#include <vpk/file_data_handler_factory.h>
#include <vpk/checking_data_handler_factory.h>

namespace fs   = boost::filesystem;
namespace algo = boost::algorithm;

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
		Exception exc((boost::format("file does not end in \"_dir.vpk\": \"%s\"") % path.string()).str());
		if (m_handler) {
			if (m_handler->archiveerror(exc, path.string())) {
				throw exc;
			}
			setName(filename);
		}
		else {
			throw exc;
		}
	}
	else {
		setName(filename.substr(0, filename.size()-8));
	}
	m_srcdir = fs::system_complete(path).parent_path().string();
	
	FileIO io(path, "rb");
	read(io);
}

void Vpk::Package::read(const std::string &srcdir, const std::string &name, FileIO &io) {
	m_srcdir = fs::system_complete(srcdir).string();
	setName(name);
	read(io);
}

void Vpk::Package::read(FileIO &io) {
	if (io.readLU32() != 0x55AA1234) {
		io.seek(-4, CUR);
	}
	else {
		unsigned int version   = io.readLU32();
		unsigned int indexSize = io.readLU32();

		if (version != 1) {
			throw FileFormatError((boost::format("unexpected vpk version %d") % version).str());
		}
	}

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

			mkpath(path).read(io, type);
		}
	}
}

Vpk::Dir &Vpk::Package::mkpath(const std::string &path) {
	std::vector<std::string> pathvec;
	algo::split(pathvec, path, algo::is_any_of("/"));
	return mkpath(pathvec);
}

Vpk::Dir &Vpk::Package::mkpath(const std::vector<std::string> &path) {
	if (path.empty()) {
		throw Exception("empty path");
	}
	
	Dir *dir = 0;
	Nodes *nodes = &this->nodes();
	for (std::vector<std::string>::const_iterator ip = path.begin(); ip != path.end(); ++ ip) {
		const std::string &name = *ip;
		Nodes::iterator in = nodes->find(name);

		if (in == nodes->end()) {
			dir = new Dir(name);
			(*nodes)[name] = NodePtr(dir);
		}
		else {
			Node *node = in->second.get();

			if (node->type() != Node::DIR) {
				std::vector<std::string> prefix;
				std::copy(path.begin(), ip+1, std::back_inserter(prefix));
				throw Exception(std::string("path is not a directory: ") + algo::join(prefix, "/"));
			}
		
			dir = (Dir*) node;
		}

		nodes = &dir->nodes();
	}

	return *dir;
}

Vpk::Node *Vpk::Package::get(const char *path) {
	if (!*path) return 0;
	
	Node  *node = this;
	Nodes *nodes = &this->nodes();
	const char *ptr = path;
	while (*ptr == '/') ++ ptr;
	while (*ptr) {
		const char *slash = strchr(ptr, '/');
		if (!slash) {
			Nodes::iterator i = nodes->find(ptr);
			if (i == nodes->end()) {
				return 0;
			}
			return i->second.get();
		}
		else {
			Nodes::iterator i = nodes->find(std::string(ptr, slash - ptr));
			if (i == nodes->end()) {
				return 0;
			}
			node = i->second.get();
			ptr = slash + 1;

			while (*ptr == '/') ++ ptr;

			if (*ptr) {
				if (node->type() != Node::DIR) {
					return 0;
				}
				nodes = &((Dir*) node)->nodes();
			}
		}
	}

	return node;

/*
	if (path == "/") return this;

	std::vector<std::string> pathvec;
	algo::split(pathvec, path, algo::is_any_of("/"));

	if (pathvec.empty()) return 0;
	// remove remnants of "/" at the beginning
	if (pathvec[0].size() == 0) pathvec.erase(pathvec.begin());

	Node  *node  = this;
	Nodes *nodes = &this->nodes();
	std::vector<std::string>::const_iterator ip = pathvec.begin();
	while (ip != pathvec.end()) {
		const std::string &name = *ip;
		Nodes::iterator in = nodes->find(name);

		if (in == nodes->end()) {
			return 0;
		}

		node = in->second.get();
		++ ip;

		if (ip != pathvec.end()) {
			if (node->type() != Node::DIR) {
				return 0;
			}
			nodes = &((Dir*) node)->nodes();
		}
	}

	return node;
	*/
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

static void list(const Vpk::Nodes &nodes, const std::vector<std::string> &prefix, std::ostream &os) {
	for (Vpk::Nodes::const_iterator it = nodes.begin(); it != nodes.end(); ++ it) {
		Vpk::Node *node = it->second.get();
		std::vector<std::string> path(prefix);
		path.push_back(node->name());
		if (node->type() == Vpk::Node::DIR) {
			list(((const Vpk::Dir*) node)->nodes(), path, os);
		}
		else {
			os << algo::join(path, "/") << std::endl;
		}
	}
}

void Vpk::Package::list(std::ostream &os) const {
	::list(nodes(), std::vector<std::string>(), os);
}

void Vpk::Package::filter(Nodes &nodes, const std::set<Node*> &keep) {
	std::vector<std::string> erase;
	for (Nodes::iterator it = nodes.begin(); it != nodes.end(); ++ it) {
		Node *node = it->second.get();
		bool kept = keep.find(node) != keep.end();
		if (node->type() == Node::DIR) {
			if (!kept) {
				Dir *dir = (Dir*) node;
				filter(dir->nodes(), keep);
				if (dir->nodes().empty()) {
					erase.push_back(node->name());
				}
			}
		}
		else if (!kept) {
			erase.push_back(node->name());
		}
	}

	for (std::vector<std::string>::const_iterator it = erase.begin(); it != erase.end(); ++ it) {
		nodes.erase(*it);
	}
}

void Vpk::Package::filter(const std::vector<std::string> &paths) {
	std::set<Node*> keep;
	for (std::vector<std::string>::const_iterator i = paths.begin(); i != paths.end(); ++ i) {
		Node *node = get(*i);
		if (node) {
			keep.insert(node);
		}
		else {
			Exception exc("no such file or directory");
			if (filtererror(exc, *i)) {
				throw exc;
			}
		}
	}

	filter(nodes(), keep);
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

boost::filesystem::path Vpk::Package::archivePath(uint16_t index) const {
	return fs::path(m_srcdir) /
		(boost::format("%s_%03d.vpk") % name() % index).str();
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
	
			if (file->size == 0) {
				try {
					dataHandler->process((char*) &file->data[0], file->data.size());
					dataHandler->finish();
				}
				catch (const std::exception &exc) {
					if (fileerror(exc, path)) throw;
					continue;
				}
			}
			else {
				boost::shared_ptr<FileIO> archive;
				
				Archives::iterator i = archives.find(file->index);
				if (i != archives.end()) {
					archive = i->second;
					if (!archive) {
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

				archive->seek(file->offset);
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
					
				if (!file->data.empty()) {
					std::string smallpath = path + ".smalldata";

					try {
						dataHandler.reset(factory.create(smallpath, file->crc32));
						dataHandler->process((char*) &file->data[0], file->data.size());
						dataHandler->finish();
					}
					catch (const std::exception& exc) {
						if (fileerror(exc, smallpath)) throw;
						continue;
					}
				}

				if (m_handler) m_handler->success(path);
			}
		}
	}
}

void Vpk::Package::process(DataHandlerFactory &factory) const {
	Archives archives;

	if (m_handler) m_handler->begin(*this);

	process(nodes(), std::vector<std::string>(), archives, factory);

	if (m_handler) m_handler->end();
}
