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
#ifndef VPK_NODE_H
#define VPK_NODE_H

#include <iostream>
#include <string>
#include <map>

#include <boost/shared_ptr.hpp>

namespace Vpk {
	class Node {
	public:
		enum Type {
			FILE,
			DIR
		};

		Node(const std::string& name) : m_name(name) {}
		virtual ~Node() {}
		
		virtual Type type() const = 0;

		void setName(const std::string &name) { m_name = name; }
		const std::string &name() const { return m_name; }

	private:
		std::string m_name;
	};

	typedef boost::shared_ptr<Node>       NodePtr;
	typedef std::map<std::string,NodePtr> Nodes;
}

#endif
