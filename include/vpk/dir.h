#ifndef VPK_DIR_H
#define VPK_DIR_H

#include <iostream>
#include <string>

#include <vpk/node.h>

namespace Vpk {
	class Dir : public Node {
	public:
		Dir(const std::string &name) : Node(name) {}

		Type type() const { return Node::DIR; }
		void read(std::istream &is, const std::string &type);

		const Nodes &nodes() const { return m_nodes; }
		      Nodes &nodes()       { return m_nodes; }
	
	private:
		Nodes m_nodes;
	};
	
	typedef boost::shared_ptr<Dir> DirPtr;
}

#endif
