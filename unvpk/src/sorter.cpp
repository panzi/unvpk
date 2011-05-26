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
#include <vpk/sorter.h>

bool Vpk::Sorter::operator () (const ListEntry &lhs, const ListEntry &rhs) const {
	for (SortKeys::const_iterator i = m_keys.begin(); i != m_keys.end(); ++ i) {
		switch (*i) {
		case SORT_ARCH: {
			int32_t larch = lhs.archive();
			int32_t rarch = rhs.archive();
			if (larch != rarch) {
				return larch < rarch;
			}
			break;
		}
		case SORT_CRC32:
			if (lhs.file->crc32 != rhs.file->crc32) {
				return lhs.file->crc32 < rhs.file->crc32;
			}
			break;
		case SORT_OFF: {
			int64_t loff = lhs.offset();
			int64_t roff = rhs.offset();
			if (loff != roff) {
				return loff < roff;
			}
			break;
		}
		case SORT_SIZE: {
			size_t lsize = lhs.file->size + lhs.file->preload.size();
			size_t rsize = rhs.file->size + rhs.file->preload.size();
			if (lsize != rsize) {
				return lsize < rsize;
			}
			break;
		}
		case SORT_PATH:
			if (lhs.path != rhs.path) {
				return lhs.path < rhs.path;
			}
			break;
		case SORT_RARCH: {
			int32_t larch = lhs.archive();
			int32_t rarch = rhs.archive();
			if (larch != rarch) {
				return larch > rarch;
			}
			break;
		}
		case SORT_RCRC32:
			if (lhs.file->crc32 != rhs.file->crc32) {
				return lhs.file->crc32 > rhs.file->crc32;
			}
			break;
		case SORT_ROFF: {
			int64_t loff = lhs.offset();
			int64_t roff = rhs.offset();
			if (loff != roff) {
				return loff > roff;
			}
			break;
		}
		case SORT_RSIZE: {
			size_t lsize = lhs.file->size + lhs.file->preload.size();
			size_t rsize = rhs.file->size + rhs.file->preload.size();
			if (lsize != rsize) {
				return lsize > rsize;
			}
			break;
		}
		case SORT_RPATH:
			if (lhs.path != rhs.path) {
				return lhs.path > rhs.path;
			}
			break;
		}
	}
	
	return false;
}
