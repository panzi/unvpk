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

		const std::string &name() const { return m_name; }

	private:
		std::string m_name;
	};

	typedef boost::shared_ptr<Node>       NodePtr;
	typedef std::map<std::string,NodePtr> Nodes;
}

#endif
