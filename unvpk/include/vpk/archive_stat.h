/**
 * unvpk - list, check and extract vpk archives
 * Copyright (C) 2013  Mathias Panzenböck <grosser.meister.morti@gmx.net>
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
#ifndef VPK_ARCHIVE_STATS_H
#define VPK_ARCHIVE_STATS_H

#include <vpk/coverage.h>
#include <vpk/file.h>

namespace Vpk {
	class ArchiveStat {
	public:
		ArchiveStat() :
			m_files(0),
			m_minPreload(-1),
			m_maxPreload(0),
			m_sumPreload(0),
			m_minSize(-1),
			m_maxSize(0),
			m_sumSize(0) {}

		void add(const File &file);

		Coverage &coverage() { return m_coverage; }
		const Coverage &coverage() const { return m_coverage; }
		size_t files() const { return m_files; }
		size_t minPreload() const { return m_minPreload; }
		size_t maxPreload() const { return m_maxPreload; }
		size_t sumPreload() const { return m_sumPreload; }
		size_t minSize() const { return m_minSize; }
		size_t maxSize() const { return m_maxSize; }
		size_t sumSize() const { return m_sumSize; }

	private:
		Coverage m_coverage;
		size_t   m_files;
		size_t   m_minPreload;
		size_t   m_maxPreload;
		size_t   m_sumPreload;
		size_t   m_minSize;
		size_t   m_maxSize;
		size_t   m_sumSize;
	};
}

#endif
