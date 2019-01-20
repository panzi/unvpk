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

#include <fuse.h>
#include <errno.h>
#include <fcntl.h>
#include <memory.h>
#include <sys/xattr.h>
#include <endian.h>
#include <stdint.h>

#include <iostream>
#include <limits>

#include <boost/lexical_cast.hpp>
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
		"    -o opt,[opt...]        mount options (see: man fuse)\n"
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

	m_archive = fs::absolute(m_archive).string();
	setup();
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
	setup();
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

#if FUSE_USE_VERSION >= 29
static int vpk_read_buf(const char *path, struct fuse_bufvec **bufp,
                        size_t size, off_t offset, struct fuse_file_info *fi) {
	return ((Vpk::Vpkfs*) fuse_get_context()->private_data)->read_buf(
		path, bufp, size, offset, fi);
}
#endif

static int vpk_statfs(const char *path, struct statvfs *stbuf) {
	return ((Vpk::Vpkfs*) fuse_get_context()->private_data)->statfs(
		path, stbuf);
}

static int vpk_listxattr(const char *path, char *buf, size_t size) {
	return ((Vpk::Vpkfs*) fuse_get_context()->private_data)->listxattr(
		path, buf, size);
}

static int vpk_getxattr(const char *path, const char *name, char *buf, size_t size) {
	return ((Vpk::Vpkfs*) fuse_get_context()->private_data)->getxattr(
		path, name, buf, size);
}

static void *vpk_init(struct fuse_conn_info *) {
	Vpk::Vpkfs *vpk = (Vpk::Vpkfs*) fuse_get_context()->private_data;
	try {
		vpk->init();
	}
	catch (const std::exception &exc) {
		std::cerr << "*** error: " << exc.what() << std::endl;
		exit(1); // can't throw through C code
	}
	catch (...) {
		std::cerr << "*** unknown exception\n";
		exit(1); // can't throw through C code
	}

	return vpk;
}

void Vpk::Vpkfs::setup() {
	memset(&m_operations, 0, sizeof(m_operations));

	m_operations.init             = vpk_init;
	m_operations.getattr          = vpk_getattr;
	m_operations.open             = vpk_open;
	m_operations.read             = vpk_read;
	m_operations.statfs           = vpk_statfs;
	m_operations.getxattr         = vpk_getxattr;
	m_operations.listxattr        = vpk_listxattr;
	m_operations.opendir          = vpk_opendir;
	m_operations.readdir          = vpk_readdir;
	m_operations.flag_nullpath_ok = 1;

#if FUSE_USE_VERSION >= 29
	m_operations.flag_nopath      = 1;
	m_operations.read_buf         = vpk_read_buf;
#endif
}

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

	return fuse_main(m_args.argc(), m_args.argv(), &m_operations, this);
}

void Vpk::Vpkfs::init() {
	clear();
	m_handler.setRaise(true);
	m_package.read(m_archive);
	m_handler.setRaise(false);

	m_files = 0;
	statfs(&m_package);

	for (Indices::const_iterator i = m_indices.begin(); i != m_indices.end(); ++ i) {
		uint16_t index = *i;
		fs::path archivePath(m_package.archivePath(index));
		int fd = ::open(archivePath.string().c_str(), O_RDONLY);
		if (fd < 0) {
			int errnum = errno;
			std::cerr
				<< "*** error opening archive \"" << archivePath << "\": "
				<< strerror(errnum) << std::endl;
			throw IOError(errnum);
		}
		m_archives[index] = fd;
	}
}

// only minimal stat:
static struct stat *vpk_stat(const Vpk::Node *node, struct stat *stbuf) {
	stbuf->st_ino = (ino_t) node;
	if (node->type() == Vpk::Node::DIR) {
		stbuf->st_mode  = S_IFDIR | 0555;
		stbuf->st_nlink = ((Vpk::Dir *) node)->subdirs() + 2;
	}
	else {
		const Vpk::File *file = (const Vpk::File*) node;
		stbuf->st_mode  = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size  = file->preload.size() + file->size;
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
		return 0;
	}
	else {
		return -errno;
	}
}

int Vpk::Vpkfs::opendir(const char *path, struct fuse_file_info *fi) {
	Node *node = m_package.get(path);
	
	if (!node) {
		return -ENOENT;
	}

	if (node->type() != Node::DIR) {
		return -ENOTDIR;
	}

	if((fi->flags & 3) != O_RDONLY) {
		return -EACCES;
	}

	fi->fh = (intptr_t) (Dir *) node;

	return 0;
}

int Vpk::Vpkfs::readdir(const char *, void *buf, fuse_fill_dir_t filler,
                        off_t, struct fuse_file_info *fi) {
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

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	fi->keep_cache = 1;
	fi->fh = (intptr_t) (File *) node;

	return 0;
}

int Vpk::Vpkfs::read(const char *, char *buf, size_t size, off_t offset,
                     struct fuse_file_info *fi) {
	if (offset < 0) return -EINVAL;

	File *file = (File *) fi->fh;

	size_t preloadSize = file->preload.size();
	size_t fileSize = preloadSize + file->size;

	if ((size_t)offset >= fileSize) return 0;

	size_t count = 0;
	if ((size_t)offset < preloadSize) {
		count = std::min(size, preloadSize - offset);
		memcpy(buf, &file->preload[offset], count);
	}

	size_t rest = std::min(size - count, fileSize - offset - count);
	if (rest) {
		int fd = m_archives[file->index];
		ssize_t restcount = pread(fd, buf + count, rest,
			file->offset + (offset + count - preloadSize));

		if (restcount < 0) {
			return -errno;
		}
		count += restcount;
	}

	return count;
}

#if FUSE_USE_VERSION >= 29
int Vpk::Vpkfs::read_buf(const char *, struct fuse_bufvec **bufp,
             size_t size, off_t offset, struct fuse_file_info *fi) {
	if (offset < 0) return -EINVAL;

	File *file = (File *) fi->fh;
	struct fuse_bufvec *bufvec = NULL;

	size_t preloadSize = file->preload.size();
	size_t fileSize = preloadSize + file->size;

	size_t count = 0;
	if ((size_t)offset >= fileSize) {
		bufvec = (struct fuse_bufvec*)calloc(1, sizeof(struct fuse_bufvec));
		if (!bufvec) return -ENOMEM;
		bufvec->buf[0].fd = -1;
	}
	else if ((size_t)offset < preloadSize) {
		count = std::min(size, preloadSize - offset);
		void *buf = malloc(count);
		if (!buf) return -ENOMEM;
		memcpy(buf, &file->preload[offset], count);

		size_t rest = std::min(size - count, fileSize - offset - count);
		if (rest) {
			bufvec = (struct fuse_bufvec*)calloc(1,
				sizeof(struct fuse_bufvec) + sizeof(struct fuse_buf));
			if (!bufvec) {
				free(buf);
				return -ENOMEM;
			}
			bufvec->count        = 2;
			bufvec->buf[0].size  = count;
			bufvec->buf[0].mem   = buf;
			bufvec->buf[0].fd    = -1;
			bufvec->buf[1].size  = rest;
			bufvec->buf[1].flags = (enum fuse_buf_flags)(FUSE_BUF_IS_FD | FUSE_BUF_FD_SEEK);
			bufvec->buf[1].fd    = m_archives[file->index];
			bufvec->buf[1].pos   = file->offset + (offset + count - preloadSize);

			count += rest;
		}
		else {
			bufvec = (struct fuse_bufvec*)calloc(1, sizeof(struct fuse_bufvec));
			if (!bufvec) {
				free(buf);
				return -ENOMEM;
			}
			bufvec->count       = 1;
			bufvec->buf[0].size = count;
			bufvec->buf[0].mem  = buf;
			bufvec->buf[0].fd   = -1;
		}
	}
	else {
		bufvec = (struct fuse_bufvec*)calloc(1, sizeof(struct fuse_bufvec));
		if (!bufvec) return -ENOMEM;
		bufvec->count        = 1;
		bufvec->buf[0].size  = count = std::min(size, fileSize - offset);
		bufvec->buf[0].flags = (enum fuse_buf_flags)(FUSE_BUF_IS_FD | FUSE_BUF_FD_SEEK);
		bufvec->buf[0].fd    = m_archives[file->index];
		bufvec->buf[0].pos   = file->offset + (offset - preloadSize);
	}

	*bufp = bufvec;
	return count;
}
#endif

int Vpk::Vpkfs::statfs(const char *, struct statvfs *stbuf) {
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

#define VPK_XATTRS_ALL \
	"user.vpkfs.dir_path"

#define VPK_XATTRS_FILES_ONLY \
	"\0user.vpkfs.crc32" \
	"\0user.vpkfs.preload_size"

#define VPK_XATTRS_ARCHIVED_ONLY \
	"\0user.vpkfs.archive_index" \
	"\0user.vpkfs.archive_path" \
	"\0user.vpkfs.offset"

#define VPK_XATTRS_DIR      VPK_XATTRS_ALL
#define VPK_XATTRS_INLINED  VPK_XATTRS_ALL VPK_XATTRS_FILES_ONLY
#define VPK_XATTRS_ARCHIVED VPK_XATTRS_INLINED VPK_XATTRS_ARCHIVED_ONLY

int Vpk::Vpkfs::listxattr(const char *path, char *buf, size_t size) {
	Node *node = m_package.get(path);

	if (!node) return -ENOENT;

	size_t xattrs_size = 0;
	const char *xattrs_list = 0;
	if (node->type() == Node::DIR) {
		xattrs_size = sizeof(VPK_XATTRS_DIR);
		xattrs_list = VPK_XATTRS_DIR;
	}
	else if (((File*) node)->size) {
		xattrs_size = sizeof(VPK_XATTRS_ARCHIVED);
		xattrs_list = VPK_XATTRS_ARCHIVED;
	}
	else {
		xattrs_size = sizeof(VPK_XATTRS_INLINED);
		xattrs_list = VPK_XATTRS_INLINED;
	}

	if (size > 0) {
		if (xattrs_size > size) {
			return -ERANGE;
		}
		memcpy(buf, xattrs_list, xattrs_size);
	}
	return xattrs_size;
}

static uint16_t tobe(uint16_t value) { return htobe16(value); }
static uint32_t tobe(uint32_t value) { return htobe32(value); }

template<typename Value>
static int getxattr(Value value, char *buf, size_t size) {
	if (size > 0) {
		if (sizeof(value) > size) {
			return -ERANGE;
		}
		*((Value*) buf) = tobe(value);
	}
	return sizeof(value);
}

static int getxattr(const std::string &value, char *buf, size_t size) {
	size_t count = value.size()+1;
	if (size > 0) {
		if (count > size) {
			return -ERANGE;
		}
		memcpy(buf, value.c_str(), count);
	}
	return count;
}

int Vpk::Vpkfs::getxattr(const char *path, const char *name, char *buf, size_t size) {
	Node *node = m_package.get(path);

	if (!node) return -ENOENT;
	
	if (strcmp(name, "user.vpkfs.dir_path") == 0) {
		return ::getxattr(m_archive, buf, size);
	}
	else if (node->type() != Node::FILE) {
		return -ENODATA;
	}
	else {
		File *file = (File*) node;
		if (strcmp(name, "user.vpkfs.crc32") == 0) {
			return ::getxattr(file->crc32, buf, size);
		}
		else if (strcmp(name, "user.vpkfs.preload_size") == 0) {
			return ::getxattr((uint16_t) file->preload.size(), buf, size);
		}
		else if (!file->size) {
			return -ENODATA;
		}
		else if (strcmp(name, "user.vpkfs.archive_index") == 0) {
			return ::getxattr(file->index, buf, size);
		}
		else if (strcmp(name, "user.vpkfs.archive_path") == 0) {
			return ::getxattr(m_package.archivePath(file->index).string(), buf, size);
		}
		else if (strcmp(name, "user.vpkfs.offset") == 0) {
			return ::getxattr(file->offset, buf, size);
		}
		else {
			return -ENODATA;
		}
	}
}

void Vpk::Vpkfs::clear() {
	for (Archives::iterator i = m_archives.begin(); i != m_archives.end(); ++ i) {
		int fd = i->second;
		if (fd >= 0 && ::close(fd) != 0) {
			int errnum = errno;
			std::cerr
				<< "*** error closing archive \""
				<< m_package.archivePath(i->first) << "\": "
				<< strerror(errnum) << std::endl;
		}
		m_archives[i->first] = -1;
	}
	m_archives.clear();
	m_indices.clear();
}
