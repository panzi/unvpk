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

#include <sstream>
#include <iostream>
#include <cstdint>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <vpk/coverage.h>

size_t Vpk::Coverage::coverage() const {
	size_t size = 0;
	for (Slices::const_iterator i = m_slices.begin(); i != m_slices.end(); ++ i) {
		size += i->second;
	}
	return size;
}

void Vpk::Coverage::add(off_t offset, size_t size) {
	if (size == 0) return;
	Slices::iterator i = m_slices.find(offset);
	bool mergeafter = false;
	if (i == m_slices.end()) {
		m_slices[offset] = size;
		mergeafter = true;
	}
	else if (size > i->second) {
		m_slices[offset] = size;
		mergeafter = true;
	}
	
	i = m_slices.find(offset);
	if (i != m_slices.begin()) {
		Slices::iterator before = i;
		-- before;
		if ((size_t)before->first + before->second >= (size_t)offset) {
			i = before;
			size = offset + size - before->first;
			offset = before->first;
		}
	}
	if (mergeafter) {
		++ i;
		size_t merged = size;
		Slices::iterator start = i;
		while (i != m_slices.end() && (size_t)offset + merged >= (size_t)i->first) {
			merged = i->first + i->second - offset;
			++ i;
		}
		if (merged != size) {
			m_slices.erase(start, i);
			m_slices[offset] = merged;
		}
	}
}

static void write_slice(std::ostream &os, off_t offset, size_t size, bool humanreadable) {
	os << offset << '-' << (offset + size) << " (";
	if (humanreadable) {
		os << Vpk::Coverage::humanReadableSize(size);
	}
	else {
		os << size;
	}
	os << ')';
}

std::string Vpk::Coverage::str(bool humanreadable) const {
	if (m_slices.empty()) return "";
	std::stringstream s;
	Slices::const_iterator i = m_slices.begin();
	write_slice(s, i->first, i->second, humanreadable);
	for (++ i; i != m_slices.end(); ++ i) {
		s << ", ";
		write_slice(s, i->first, i->second, humanreadable);
	}
	return s.str();
}

Vpk::Coverage Vpk::Coverage::invert(size_t filesize) const {
	Coverage inverted;

	size_t start = 0;
	for (Slices::const_iterator i = m_slices.begin(); i != m_slices.end(); ++ i) {
		size_t offset = i->first;
		inverted.add(start, offset - start);
		start = offset + i->second;
	}

	if (filesize > start) {
		inverted.add(start, filesize - start);
	}

	return inverted;
}

std::string Vpk::Coverage::humanReadableSize(size_t size) {
	if (size < 1024) {
		return boost::lexical_cast<std::string>(size);
	}
	else if (size < 1024L * 1024) {
		return (boost::format("%.1lfK") % (size / (double)1024)).str();
	}
	else if (size < 1024L * 1024 * 1024) {
		return (boost::format("%.1lfM") % (size / (double)(1024L * 1024))).str();
	}
#if SIZE_MAX > 0xffffffff
	else if (size < 1024LL * 1024 * 1024 * 1024) {
		return (boost::format("%.1lfG") % (size / (double)(1024L * 1024 * 1024))).str();
	}
	else if (size < 1024LL * 1024 * 1024 * 1024 * 1024) {
		return (boost::format("%.1lfT") % (size / (double)(1024LL * 1024 * 1024 * 1024))).str();
	}
	else if (size < 1024LL * 1024 * 1024 * 1024 * 1024 * 1024) {
		return (boost::format("%.1lfP") % (size / (double)(1024LL * 1024 * 1024 * 1024 * 1024))).str();
	}
	else {
		return (boost::format("%.1lfE") % (size / (double)(1024LL * 1024 * 1024 * 1024 * 1024 * 1024))).str();
	}
#else
	else {
		return (boost::format("%.1lfG") % (size / (double)(1024L * 1024 * 1024))).str();
	}
#endif
}
