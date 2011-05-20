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
#include <vpk/dir.h>
#include <vpk/file.h>

void Vpk::Dir::read(FileReader &reader, const std::string &type) {
	// files
	for (;;) {
		std::string name;
		reader.readAsciiZ(name);
		if (name.empty()) break;

		name += ".";
		name += type;
		File *file = new File(name);
		m_nodes[name] = NodePtr(file);
		file->read(reader);
	}
}
