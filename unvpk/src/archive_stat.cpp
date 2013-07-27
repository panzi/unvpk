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

#include <vpk/archive_stat.h>

void Vpk::ArchiveStat::add(const File &file) {
	size_t preload = file.preload.size();
	size_t size = preload + file.size;
	m_sumPreload += preload;
	m_sumSize    += size;
	if (m_files == 0) {
		m_minPreload = m_maxPreload = preload;
		m_minSize    = m_maxSize    = size;
	}
	else {
		if (preload > m_maxPreload) m_maxPreload = preload;
		if (preload < m_minPreload) m_minPreload = preload;
		
		if (size > m_maxSize) m_maxSize = size;
		if (size < m_minSize) m_minSize = size;
	}
	++ m_files;
	m_coverage.add(file.offset, file.size);
}
