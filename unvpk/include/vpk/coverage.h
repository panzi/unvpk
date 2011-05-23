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
#ifndef VPK_COVERAGE_H
#define VPK_COVERAGE_H

#include <string>
#include <map>

namespace Vpk {
	class Coverage {
	public:
		typedef std::map<off_t,size_t> Slices;

		size_t coverage() const;
		const Slices &slices() const { return m_slices; }
		
		void add(off_t offset, size_t size);
		std::string str(bool humanreadable = false) const;
		bool empty() const { return m_slices.empty(); }
		Coverage invert(size_t filesize = 0) const;

		static std::string humanReadableSize(size_t size);

	private:
		Slices m_slices;
	};
}

#endif
