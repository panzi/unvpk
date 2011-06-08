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
#ifndef VPK_DIR_H
#define VPK_DIR_H

#include <string>
#include <vector>

#include <vpk/node.h>
#include <vpk/file_io.h>

namespace Vpk {
	class File;

	class Dir : public Node {
	public:
		typedef Nodes::iterator iterator;
		typedef Nodes::const_iterator const_iterator;

		Dir(const std::string &name) : Node(name), m_subdirs(0) {}

		Type type() const { return Node::DIR; }
		void read(FileIO &io, const std::string &path, const std::string &type, std::vector<File*> &dirfiles);

		const Nodes &nodes() const { return m_nodes; }
		const Node *node(const std::string &name) const;
		      Node *node(const std::string &name);
		void add(Node *node);
		void remove(const std::string &name);

		iterator begin() { return m_nodes.begin(); }
		iterator end()   { return m_nodes.end(); }
		const_iterator begin() const { return m_nodes.begin(); }
		const_iterator end()   const { return m_nodes.end(); }
		bool empty() const { return m_nodes.empty(); }
	
		// only used by vpkfs so it can give a UNIX-like
		// hardlink count so find works:
		size_t subdirs() const { return m_subdirs; }

	private:
		Nodes  m_nodes;
		size_t m_subdirs;
	};
}

#endif
