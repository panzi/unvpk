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
#include <iostream>

#include <vpk/dir.h>
#include <vpk/file.h>

void Vpk::Dir::read(FileIO &io, const std::string &path, const std::string &type, std::vector<File*> &dirfiles) {
	// files
	for (;;) {
		std::string name;
		io.readAsciiZ(name);
		if (name.empty()) break;

		name += ".";
		name += type;
		if (m_nodes.find(name) != m_nodes.end()) {
			std::cerr
				<< "*** warning: file occured more than once: \""
				<< path << "/" << name << "\"\n";
		}
		File *file = new File(name);
		m_nodes[name] = NodePtr(file);
		file->read(io, dirfiles);
	}
}

const Vpk::Node *Vpk::Dir::node(const std::string &name) const {
	Nodes::const_iterator i = m_nodes.find(name);
	if (i == m_nodes.end()) {
		return 0;
	}
	else {
		return i->second.get();
	}
}

Vpk::Node *Vpk::Dir::node(const std::string &name) {
	Nodes::iterator i = m_nodes.find(name);
	if (i == m_nodes.end()) {
		return 0;
	}
	else {
		return i->second.get();
	}
}

void Vpk::Dir::add(Node *node) {
	m_nodes[node->name()] = NodePtr(node);
	if (node->type() == DIR) {
		++ m_subdirs;
	}
}

void Vpk::Dir::remove(const std::string &name) {
	Nodes::iterator i = m_nodes.find(name);
	if (i != m_nodes.end()) {
		if (i->second->type() == DIR) {
			-- m_subdirs;
		}
		m_nodes.erase(i);
	}
}
