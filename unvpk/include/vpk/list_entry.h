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
#ifndef VPK_LIST_ENTRY_H
#define VPK_LIST_ENTRY_H

#include <string>

#include <boost/format.hpp>

#include <vpk/file.h>
#include <vpk/console_table.h>

namespace Vpk {
	class ListEntry {
	public:
		ListEntry(const std::string &path, File *file)
			: path(path), file(file) {}
		
		template<typename SizeFormatter>
		void insert(ConsoleTable &table, SizeFormatter szfmt) const {
			size_t size = file->preload.size() + file->size;
			
			if (file->size) {
				table.row(file->index,
					boost::format("%08x") % file->crc32, szfmt(file->offset), szfmt(size), path);
			}
			else {
				table.row("-",
					boost::format("%08x") % file->crc32, "-", szfmt(size), path);
			}
		}
	
		int32_t archive() const {
			if (file->size) {
				return file->index;
			}
			else {
				return -1;
			}
		}
	
		int64_t offset() const {
			if (file->size) {
				return file->offset;
			}
			else {
				return -1;
			}
		}
	
		std::string path;
		File *file;
	};
	
	typedef std::vector<ListEntry> List;
}

#endif
