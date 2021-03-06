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
#ifndef VPK_PACKAGE_H
#define VPK_PACKAGE_H

#include <iostream>
#include <vector>
#include <set>
#include <map>

#include <boost/unordered_map.hpp>
#include <boost/filesystem/operations.hpp>

#include <vpk/node.h>
#include <vpk/dir.h>
#include <vpk/handler.h>
#include <vpk/data_handler_factory.h>
#include <vpk/file_io.h>

namespace Vpk {
	class File;

	class Package : public Dir {
	public:
		Package(Handler *handler = 0) :
			Dir(""), m_version(0), m_dataOffset(0), m_footerOffset(0), m_footerSize(0), m_srcdir("."), m_handler(handler) {}

		void read(const char *path) { read(boost::filesystem::path(path)); }
		void read(const std::string &path) { read(boost::filesystem::path(path)); }
		void read(const boost::filesystem::path &path);
		void read(const char *path, FileIO &io) { read(boost::filesystem::path(path), io); }
		void read(const std::string &path, FileIO &io) { read(boost::filesystem::path(path), io); }
		void read(const boost::filesystem::path &path, FileIO &io);

		Dir &mkpath(const std::string &path) { return mkpath(path.c_str()); }
		Dir &mkpath(const char *path);

		std::string             archiveName(uint16_t index) const;
		boost::filesystem::path archivePath(uint16_t index) const;
		unsigned int version() const { return m_version; }
		unsigned int dataoff() const { return m_dataOffset; }
		unsigned int footerOffset() const { return m_footerOffset; }
		unsigned int footerSize() const { return m_footerSize; }
		const std::string &srcdir() const { return m_srcdir; }
		const std::string &dirfile() const { return m_dirfile; }
		Node *get(const std::string &path) { return get(path.c_str()); }
		Node *get(const char *path);
		void setHandler(Handler *handler) { m_handler = handler; }
		const Handler *handler() const { return m_handler; }

		void filter(const std::vector<std::string> &paths);
		void extract(const std::string &destdir, bool check = false) const;
		void check() const;
		void process(DataHandlerFactory &factory) const;

		size_t filecount() const;

		typedef boost::unordered_map< uint16_t, boost::shared_ptr<FileIO> > Archives;

	private:
		typedef bool (Handler::*ErrorMethod)(const std::exception &exc, const std::string &path);

		void read(FileIO &io);
		void filter(Dir &dir, const std::set<Node*> &keep);
		void process(const Nodes &nodes, const std::vector<std::string> &prefix, Archives &archives, DataHandlerFactory &factory) const;

		bool direrror(const std::exception &exc, const std::string &path)     const { return error(exc, path, &Handler::direrror); }
		bool fileerror(const std::exception &exc, const std::string &path)    const { return error(exc, path, &Handler::fileerror); }
		bool archiveerror(const std::exception &exc, const std::string &path) const { return error(exc, path, &Handler::archiveerror); }
		bool filtererror(const std::exception &exc, const std::string &path)  const { return error(exc, path, &Handler::filtererror); }

		bool direrror(const std::string &msg, const std::string &path)     const { return error(msg, path, &Handler::direrror); }
		bool fileerror(const std::string &msg, const std::string &path)    const { return error(msg, path, &Handler::fileerror); }
		bool archiveerror(const std::string &msg, const std::string &path) const { return error(msg, path, &Handler::archiveerror); }
		bool filtererror(const std::string &msg, const std::string &path)  const { return error(msg, path, &Handler::filtererror); }

		bool error(const std::string &msg, const std::string &path, ErrorMethod handler) const;
		bool error(const std::exception &exc, const std::string &path, ErrorMethod handler) const;

		unsigned int m_version;
		unsigned int m_dataOffset;
		unsigned int m_footerOffset;
		unsigned int m_footerSize;
		std::string  m_srcdir;
		std::string  m_dirfile;
		Handler     *m_handler;
	};
}

#endif
