/**
 * vpkfs - mount vpk archives
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
#ifndef VPK_FUSE_FUSE_ARGS_H
#define VPK_FUSE_FUSE_ARGS_H

#include <string>

#include <string.h>
#include <errno.h>

#include <fuse.h>

#include <vpk/exception.h>

namespace Vpk {
	class FuseArgs {
	public:
		FuseArgs() {
			m_args.argc = 0;
			m_args.argv = 0;
			m_args.allocated = 0;
		}
		FuseArgs(int argc, char *argv[], bool allocated = false) {
			m_args.argc = argc;
			m_args.argv = argv;
			m_args.allocated = allocated;
		}
		~FuseArgs() {
			fuse_opt_free_args(&m_args);
		}
	
		template<typename Data>
		void parse(Data *data, const struct fuse_opt opts[],
		          int (*proc)(Data *data, const char *arg, int key,
			                  struct fuse_args *outargs)) {
			if (fuse_opt_parse(&m_args, data, opts, (fuse_opt_proc_t) proc) != 0) {
				throw Exception(std::string("error parsing options: ") + strerror(errno));
			}
		}
	
		void add_arg(const char *arg) {
			if (fuse_opt_add_arg(&m_args, arg) != 0) {
				throw std::bad_alloc();
			}
		}
		
		void insert_arg(int pos, const char *arg) {
			if (fuse_opt_insert_arg(&m_args, pos, arg) != 0) {
				throw std::bad_alloc();
			}
		}
	
		void add_arg   (const std::string &arg) { add_arg(arg.c_str()); }
		void insert_arg(const std::string &arg) { insert_arg(arg.c_str()); }
	
		int argc() const { return m_args.argc; }
		char const* const* argv() const { return m_args.argv; }
		char** argv() { return m_args.argv; }
	
	private:
		struct fuse_args m_args;
	};
}

#endif
