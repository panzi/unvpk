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

#include <fstream>

#include <fuse.h>

#include <vpk/console_handler.h>
#include <vpk/package.h>

// TODO: improve read performance
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
		int release(const char *path, struct fuse_file_info *fi) { return 0; }
	
	private:
//		typedef std::unordered_map< int, boost::shared_ptr<Filehandle> > Filemap;
//		typedef std::unordered_map< std::string, int > Descrs;

		struct fuse_args   m_args;
		bool               m_run;
		std::string        m_archive;
		ConsoleHandler     m_handler;
		Package            m_package;
		Package::Archives  m_archives;
//		Filemap            m_filemap;
//		Descrs             m_descrs;
	};
}

#endif
