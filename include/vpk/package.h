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

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <vpk/node.h>
#include <vpk/handler.h>
#include <vpk/data_handler_factory.h>

namespace Vpk {
	class Dir;
	class File;

	class Package {
	public:
		Package(Handler *handler = 0) :
			m_name(), m_srcdir("."), m_nodes(), m_handler(handler) {}

		void read(const char *path) { read(boost::filesystem::path(path)); }
		void read(const std::string &path) { read(boost::filesystem::path(path)); }
		void read(const boost::filesystem::path &path);
		void read(const std::string &srcdir, const std::string &name, std::istream &is);

		Dir &mkpath(const std::string &path);
		Dir &mkpath(const std::vector<std::string> &path);

		const std::string &name() const { return m_name; }
		const std::string &srcdir() const { return m_srcdir; }
		const Nodes &nodes() const { return m_nodes; }
		      Nodes &nodes()       { return m_nodes; }
		      Node  *get(const std::string &path);
		void setHandler(Handler *handler) { m_handler = handler; }
		const Handler *handler() const { return m_handler; }

		void filter(const std::vector<std::string> &paths);

		void list(std::ostream &os = std::cout) const;
		void list(const std::vector<std::string> &filter, std::ostream &os = std::cout) const;

		void extract(const std::string &destdir, bool check = false) const;
		void check() const;
		void process(DataHandlerFactory &factory) const;

		size_t filecount() const;

	private:
		typedef std::map<std::string, boost::shared_ptr<boost::filesystem::ifstream> > Archives;
		typedef bool (Handler::*ErrorMethod)(const std::exception &exc, const std::string &path);

		void read(std::istream &is);
		void filter(Nodes &nodes, const std::set<Node*> &keep);
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

		std::string m_name;
		std::string m_srcdir;
		Nodes       m_nodes;
		Handler    *m_handler;
	};
}

#endif
