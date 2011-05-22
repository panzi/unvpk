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

#define FUSE_USE_VERSION  26

#include <fuse.h>
#include <errno.h>
#include <fcntl.h>
#include <memory.h>

#include <iostream>
#include <limits>

#include <boost/filesystem/operations.hpp>

#include <vpk/version.h>
#include <vpk/file.h>
#include <vpk/vpkfs.h>
#include <vpk/fuse_args.h>
#include <vpk/io_error.h>

namespace fs = boost::filesystem;

enum {
	VPK_OPTS_OK      = 0,
	VPK_OPTS_HELP    = 1,
	VPK_OPTS_VERSION = 2,
	VPK_OPTS_ERROR   = 4
};

struct vpkfuse_config {
	vpkfuse_config(
		std::string &archive,
		std::string &mountpoint,
		int &flags)
	: archive(archive),
	  mountpoint(mountpoint),
	  argind(0),
	  flags(flags) {}

	std::string &archive;
	std::string &mountpoint;
	int argind;
	int &flags;
};

enum {
	KEY_HELP,
	KEY_VERSION
};

static struct fuse_opt vpkfuse_opts[] = {
	FUSE_OPT_KEY("-v",        KEY_VERSION),
	FUSE_OPT_KEY("--version", KEY_VERSION),
	FUSE_OPT_KEY("-h",        KEY_HELP),
	FUSE_OPT_KEY("--help",    KEY_HELP),
	FUSE_OPT_END
};

void usage(const char *binary) {
	std::cout << "Usage: " << binary << " [OPTIONS] ARCHIVE MOUNTPOINT\n"
		"Mount VPK archives.\n"
		"ARCHIVE has to be a file named \"*_dir.vpk\".\n"
		"This filesystem is read-only and only supports blocking operations.\n"
		"\n"
		"Options:\n"
		"    -o opt,[opt...]        mount options\n"
		"    -h   --help            print help\n"
		"    -v   --version         print version\n"
		"    -d   -o debug          enable debug output (implies -f)\n"
		"    -f                     foreground operation\n"
		"    -s                     disable multi-threaded operation\n"
		"                           Note that this might actually increase\n"
		"                           performance in case you access the\n"
		"                           filesystem with only one process.\n"
		"\n"
		"(c) 2011 Mathias Panzenböck\n";
}

static int vpkfuse_opt_proc(struct vpkfuse_config *conf, const char *arg, int key, struct fuse_args *outargs) {
	switch (key) {
	case FUSE_OPT_KEY_NONOPT:
		switch (conf->argind) {
		case 0:
			conf->archive = arg;
			++ conf->argind;
			return 0;

		case 1:
			conf->mountpoint = arg;
			++ conf->argind;
			break;

		default:
			std::cerr << "*** error: to many arguments\n";
			usage(outargs->argv[0]);
			conf->flags |= VPK_OPTS_ERROR;
			++ conf->argind;
		}
		break;

	case KEY_HELP:
		usage(outargs->argv[0]);
		conf->flags |= VPK_OPTS_HELP;
		break;

	case KEY_VERSION:
		std::cout << "vpkfs version " << Vpk::VERSION << std::endl;
		conf->flags |= VPK_OPTS_VERSION;
		break;
	}
	return 1;
}

Vpk::Vpkfs::Vpkfs(int argc, char *argv[], bool allocated)
		: m_args(argc, argv, allocated),
		  m_flags(VPK_OPTS_OK),
		  m_handler(true),
		  m_package(&this->m_handler),
		  m_files(0) {
	struct vpkfuse_config conf(m_archive, m_mountpoint, m_flags);
	m_args.parse(&conf, vpkfuse_opts, vpkfuse_opt_proc);
	
	if (m_flags == VPK_OPTS_OK) {
		if (conf.argind < 1) {
			std::cerr << "*** error: required argument ARCHIVE is missing.\n";
			usage(argv[0]);
			m_flags |= VPK_OPTS_ERROR;
		}
		else if (conf.argind < 2) {
			std::cerr << "*** error: required argument MOUNTPOINT is missing.\n";
			usage(argv[0]);
			m_flags |= VPK_OPTS_ERROR;
		}
	}
}

Vpk::Vpkfs::Vpkfs(
	const std::string &archive,
	const std::string &mountpoint,
	bool               singlethreaded,
	const std::string &mountopts)
		: m_flags(VPK_OPTS_OK),
		  m_archive(archive),
		  m_mountpoint(mountpoint),
		  m_handler(true),
		  m_package(&this->m_handler),
		  m_files(0) {
	m_args.add_arg("vpkfs");
	if (singlethreaded) {
		m_args.add_arg("-s");
	}
	if (mountopts.size() > 0) {
		m_args.add_arg("-o");
		m_args.add_arg(mountopts);
	}
	m_args.add_arg("--");
	m_args.add_arg(m_mountpoint);
}

static int vpk_getattr(const char *path, struct stat *stbuf) {
	return ((Vpk::Vpkfs*) fuse_get_context()->private_data)->getattr(
		path, stbuf);
}

static int vpk_opendir(const char *path, struct fuse_file_info *fi) {
	return ((Vpk::Vpkfs*) fuse_get_context()->private_data)->opendir(
		path, fi);
}

static int vpk_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi) {
	return ((Vpk::Vpkfs*) fuse_get_context()->private_data)->readdir(
		path, buf, filler, offset, fi);
}

static int vpk_open(const char *path, struct fuse_file_info *fi) {
	return ((Vpk::Vpkfs*) fuse_get_context()->private_data)->open(
		path, fi);
}

static int vpk_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi) {
	return ((Vpk::Vpkfs*) fuse_get_context()->private_data)->read(
		path, buf, size, offset, fi);
}

static int vpk_statfs(const char *path, struct statvfs *stbuf) {
	return ((Vpk::Vpkfs*) fuse_get_context()->private_data)->statfs(
		path, stbuf);
}

static struct fuse_operations vpkfuse_operations = {
	/* getattr          */ vpk_getattr,
	/* readlink         */ 0,
	/* getdir           */ 0,
	/* mknod            */ 0,
	/* mkdir            */ 0,
	/* unlink           */ 0,
	/* rmdir            */ 0,
	/* symlink          */ 0,
	/* rename           */ 0,
	/* link             */ 0,
	/* chmod            */ 0,
	/* chown            */ 0,
	/* truncate         */ 0,
	/* utime            */ 0,
	/* open             */ vpk_open,
	/* read             */ vpk_read,
	/* write            */ 0,
	/* statfs           */ vpk_statfs,
	/* flush            */ 0,
	/* release          */ 0,
	/* fsync            */ 0,
	/* setxattr         */ 0,
	/* getxattr         */ 0,
	/* listxattr        */ 0,
	/* removexattr      */ 0,
	/* opendir          */ vpk_opendir,
	/* readdir          */ vpk_readdir,
	/* releasedir       */ 0,
	/* fsyncdir         */ 0,
	/* init             */ 0,
	/* destroy          */ 0,
	/* access           */ 0,
	/* create           */ 0,
	/* ftruncate        */ 0,
	/* fgetattr         */ 0,
	/* lock             */ 0,
	/* utimens          */ 0,
	/* bmap             */ 0,
	/* flag_nullpath_ok */ 0,
	/* flag_reserved    */ 0,
	/* ioctl            */ 0,
	/* poll             */ 0
};

void Vpk::Vpkfs::statfs(const Node *node) {
	++ m_files;
	if (node->type() == Node::DIR) {
		const Nodes &nodes = ((const Dir*) node)->nodes();
		for (Nodes::const_iterator i = nodes.begin(); i != nodes.end(); ++ i) {
			statfs(i->second.get());
		}
	}
	else {
		uint16_t arch = ((const File*) node)->index;
		if (m_indices.find(arch) == m_indices.end()) {
			m_indices.insert(arch);
		}
	}
}

int Vpk::Vpkfs::run() {
	if (m_flags & VPK_OPTS_ERROR) return 1;
	if (m_flags & (VPK_OPTS_HELP | VPK_OPTS_VERSION)) return 0;

	clear();
	m_handler.setRaise(true);
	m_package.read(m_archive);
	m_handler.setRaise(false);

	m_files = 0;
	statfs(&m_package);
	int archspace = 0;
	for (Indices::const_iterator i = m_indices.begin(); i != m_indices.end(); ++ i) {
		int needspace = *i + 1;
		if (needspace > archspace) {
			archspace = needspace;
		}
	}
	
	m_archives.resize(archspace, -1);
	for (Indices::const_iterator i = m_indices.begin(); i != m_indices.end(); ++ i) {
		uint16_t index = *i;
		fs::path archivePath(m_package.archivePath(index));
		int fd = ::open(archivePath.string().c_str(), O_RDONLY);
		if (fd < 0) {
			int errnum = errno;
			std::cerr
				<< "*** error opening archive: \""
				<< archivePath << "\"\n";
			throw IOError(errnum);
		}
		m_archives[index] = fd;
	}

	return fuse_main(m_args.argc(), m_args.argv(), &vpkfuse_operations, this);
}

// only minimal stat:
static struct stat *vpk_stat(const Vpk::Node *node, struct stat *stbuf) {
	stbuf->st_ino = (ino_t) node;
	if (node->type() == Vpk::Node::DIR) {
		stbuf->st_mode  = S_IFDIR | 0555;
		stbuf->st_nlink = 2;
	}
	else {
		const Vpk::File *file = (const Vpk::File*) node;
		stbuf->st_mode  = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size  = file->size ? file->size : file->data.size();
	}
	return stbuf;
}

int Vpk::Vpkfs::getattr(const char *path, struct stat *stbuf) {
	Node *node = m_package.get(path);
	memset(stbuf, 0, sizeof(struct stat));

	if (!node) {
		return -ENOENT;
	}

	vpk_stat(node, stbuf);

	struct stat archst;
	int code = 0;
	if (node->type() == Vpk::Node::FILE && ((File*) node)->size) {
		int fd = m_archives[((File*) node)->index];
		code = fstat(fd, &archst);
	}
	else {
		code = stat(m_archive.c_str(), &archst);
	}

	if (code == 0) {
		stbuf->st_uid = archst.st_uid;
		stbuf->st_gid = archst.st_gid;
		stbuf->st_blksize = archst.st_blksize;
		stbuf->st_atime = archst.st_atime;
		stbuf->st_ctime = archst.st_ctime;
		stbuf->st_mtime = archst.st_mtime;
	}

	return -code;
}

int Vpk::Vpkfs::opendir(const char *path, struct fuse_file_info *fi) {
	Node *node = m_package.get(path);
	
	if (!node) {
		return -ENOENT;
	}

	if (node->type() != Node::DIR) {
		return -ENOTDIR;
	}

	if((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	fi->fh = (uint64_t) (Dir *) node;

	return 0;
}

int Vpk::Vpkfs::readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                        off_t offset, struct fuse_file_info *fi) {
	Dir *dir = (Dir *) fi->fh;
	
	struct stat stbuf;
	memset(&stbuf, 0, sizeof(struct stat));

	if (filler(buf, ".", vpk_stat(dir, &stbuf), 0)) return 0;
	if (filler(buf, "..", NULL, 0)) return 0;

	const Nodes &nodes = dir->nodes();
	for (Nodes::const_iterator i = nodes.begin(); i != nodes.end(); ++ i) {
		const Node *child = i->second.get();
		if (filler(buf, child->name().c_str(), vpk_stat(child, &stbuf), 0)) return 0;
	}

	return 0;
}

int Vpk::Vpkfs::open(const char *path, struct fuse_file_info *fi) {
	Node *node = m_package.get(path);
	
	if (!node)
		return -ENOENT;

	if (node->type() == Node::DIR)
		return -EISDIR;

	if((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	fi->fh = (uint64_t) (File *) node;

	return 0;
}

int Vpk::Vpkfs::read(const char *path, char *buf, size_t size, off_t offset,
                     struct fuse_file_info *fi) {
	File *file = (File *) fi->fh;

	if (file->size) {
		if (offset >= file->size) {
			return 0;
		}
		int fd = m_archives[file->index];
		size_t n = std::min(size, (size_t)(file->size - offset));
		ssize_t count = pread(fd, buf, n, file->offset + offset);

		if (count < 0) {
			return -errno;
		}
		
		return count;
	}
	else {
		if (offset >= file->data.size()) {
			return 0;
		}
		size_t n = std::min(size, file->data.size() - offset);
		memcpy(buf, &file->data[offset], n);
		return n;
	}
}

int Vpk::Vpkfs::statfs(const char *path, struct statvfs *stbuf) {
	struct stat archst;
	fsfilcnt_t fssize = 0;
	memset(stbuf, 0, sizeof(struct statvfs));

	int code = stat(m_archive.c_str(), &archst);
	if (code != 0) {
		return code;
	}

	fssize = archst.st_size;
	stbuf->f_bsize   = archst.st_blksize;
	stbuf->f_files   = m_files;
	stbuf->f_namemax = std::numeric_limits<unsigned long>::max();
	
	for (boost::unordered_set<uint16_t>::const_iterator i = m_indices.begin();
			i != m_indices.end(); ++ i) {
		code = stat(m_package.archivePath(*i).string().c_str(), &archst);

		if (code != 0) {
			return code;
		}

		fssize += archst.st_size;
	}
	
	// ceiling integer division:
	stbuf->f_blocks = (fssize + stbuf->f_bsize - 1) / stbuf->f_bsize;

	return 0;
}

void Vpk::Vpkfs::close(uint16_t index) {
	int fd = m_archives[index];
	if (::close(fd) != 0) {
		int errnum = errno;
		std::cerr
			<< "*** error closing archive \""
			<< m_package.archivePath(index) << "\": "
			<< strerror(errnum) << std::endl;
	}
	m_archives[index] = -1;
}

void Vpk::Vpkfs::clear() {
	for (Archives::iterator i = m_archives.begin(); i != m_archives.end(); ++ i) {
		close(*i);
	}
	m_archives.clear();
	m_indices.clear();
}
