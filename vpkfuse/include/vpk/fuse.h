/**
 * vpkfuse - mount vpk archives
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
#ifndef VPK_FUSE_H
#define VPK_FUSE_H

#include <sys/statvfs.h>

#include <fstream>

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

#include <fuse.h>

#include <vpk/console_handler.h>
#include <vpk/package.h>

namespace Vpk {
	class Fuse {
	public:
		Fuse(int argc, char *argv[], bool allocated=false);

		int run();
		int getattr(const char *path, struct stat *stbuf);
		int readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		            off_t offset, struct fuse_file_info *fi);
		int open(const char *path, struct fuse_file_info *fi);
		int read(const char *path, char *buf, size_t size, off_t offset,
		         struct fuse_file_info *fi);
		int release(const char *path, struct fuse_file_info *fi);
		int statfs(const char *path, struct statvfs *stbuf);

		boost::shared_ptr<boost::filesystem::ifstream> archive(uint16_t index);
	
	private:
		void statfs(const Node *node);

		typedef boost::unordered_map< uint64_t, std::pair<std::string, File*> > Filemap;
		typedef boost::unordered_map< std::string, uint64_t > Descrs;
		typedef boost::unordered_map< uint16_t, boost::shared_ptr<boost::filesystem::ifstream> > Archives;

		struct fuse_args m_args;
		bool             m_run;
		std::string      m_archive;
		ConsoleHandler   m_handler;
		Package          m_package;
		Archives         m_archives;
		Filemap          m_filemap;
		Descrs           m_descrs;
		fsfilcnt_t       m_files;
		boost::unordered_set<uint16_t> m_indices;
	};
}

#endif
