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

#define FUSE_USE_VERSION  26

#include <fuse.h>
#include <errno.h>
#include <fcntl.h>
#include <memory.h>

#include <iostream>
#include <limits>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <vpk/version.h>
#include <vpk/file.h>
#include <vpk/fuse.h>

namespace fs = boost::filesystem;

struct vpkfuse_config {
	char *archive;
	bool  run;
};

enum {
	KEY_HELP,
	KEY_VERSION
};

#define VPK_OPT(t, p, v) { t, offsetof(struct vpkfuse_config, p), v }

static struct fuse_opt vpkfuse_opts[] = {
	VPK_OPT("archive=%s", archive, 0),
	
	FUSE_OPT_KEY("-v",        KEY_VERSION),
	FUSE_OPT_KEY("--version", KEY_VERSION),
	FUSE_OPT_KEY("-h",        KEY_HELP),
	FUSE_OPT_KEY("--help",    KEY_HELP),
};

static int vpkfuse_opt_proc(void *data, const char *arg, int key, struct fuse_args *outargs) {
	switch (key) {
	case KEY_HELP:
		std::cout << "Usage: " << outargs->argv[0] << " MOUNTPOINT [OPTIONS]\n"
			"\n"
			"General Options:\n"
			"    -o opt,[opt...]     mount options\n"
			"    -h   --help         print help\n"
			"    -c   --version      print version\n"
			"\n"
			"VPK Options:\n"
			"    -o archive=ARCHIVE  the VPK archive that shall be mounted\n";
		((struct vpkfuse_config*) data)->run = false;
		return 0;

	case KEY_VERSION:
		std::cout << "vpkfuse version " << Vpk::VERSION << std::endl;
		((struct vpkfuse_config*) data)->run = false;
     }
     return 1;
}

Vpk::Fuse::Fuse(int argc, char *argv[], bool allocated)
		: m_run(true),
		  m_handler(true),
		  m_package(&this->m_handler) {
	m_args.argc = argc;
	m_args.argv = argv;
	m_args.allocated = allocated;

	struct vpkfuse_config conf = { NULL, true };

	fuse_opt_parse(&m_args, &conf, vpkfuse_opts, vpkfuse_opt_proc);

	if (conf.archive) {
		m_archive = fs::system_complete(conf.archive).string();
		m_run     = conf.run;
	}
	else {
		std::cerr << "*** error: required option 'archive' is missing.\n";
		m_run = false;
	}
}

static int vpk_getattr(const char *path, struct stat *stbuf) {
	return ((Vpk::Fuse*) fuse_get_context()->private_data)->getattr(
		path, stbuf);
}

static int vpk_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi) {
	return ((Vpk::Fuse*) fuse_get_context()->private_data)->readdir(
		path, buf, filler, offset, fi);
}

static int vpk_open(const char *path, struct fuse_file_info *fi) {
	return ((Vpk::Fuse*) fuse_get_context()->private_data)->open(
		path, fi);
}

static int vpk_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi) {
	return ((Vpk::Fuse*) fuse_get_context()->private_data)->read(
		path, buf, size, offset, fi);
}

static int vpk_release(const char *path, struct fuse_file_info *fi) {
	return ((Vpk::Fuse*) fuse_get_context()->private_data)->release(
		path, fi);
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
	/* statfs           */ 0,
	/* flush            */ 0,
	/* release          */ vpk_release,
	/* fsync            */ 0,
	/* setxattr         */ 0,
	/* getxattr         */ 0,
	/* listxattr        */ 0,
	/* removexattr      */ 0,
	/* opendir          */ 0,
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

int Vpk::Fuse::run() {
	if (!m_run) return 0;

	m_handler.setRaise(true);
	m_package.read(m_archive);
	m_handler.setRaise(false);

	return fuse_main(m_args.argc, m_args.argv, &vpkfuse_operations, this);
}

// only minimal stat:
static struct stat * vpk_stat(const Vpk::Node *node, struct stat *stbuf) {
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

int Vpk::Fuse::getattr(const char *path, struct stat *stbuf) {
	Node *node = m_package.get(path);
	memset(stbuf, 0, sizeof(struct stat));

	if (!node) {
		return -ENOENT;
	}

	vpk_stat(node, stbuf);
	std::string archive;

	if (node->type() == Vpk::Node::FILE && ((File*) node)->size) {
		archive = archivePath(((File*) node)->index).string();
	}
	else {
		archive = m_archive;
	}

	struct stat archst;
	int code = stat(archive.c_str(), &archst);

	if (code == 0) {
		stbuf->st_atime = archst.st_atime;
		stbuf->st_ctime = archst.st_ctime;
		stbuf->st_mtime = archst.st_mtime;
	}

	return code;
}

int Vpk::Fuse::readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi) {
	Node *node = m_package.get(path);
	
	if (!node) {
		return -ENOENT;
	}

	if (node->type() != Node::DIR) {
		return -ENOTDIR;
	}

	struct stat stbuf;
	memset(&stbuf, 0, sizeof(struct stat));

	if (filler(buf, ".", vpk_stat(node, &stbuf), 0)) return 0;
	if (filler(buf, "..", NULL, 0)) return 0;

	const Nodes &nodes = ((Dir*) node)->nodes();
	for (Nodes::const_iterator i = nodes.begin(); i != nodes.end(); ++ i) {
		const Node *child = i->second.get();
		if (filler(buf, child->name().c_str(), vpk_stat(child, &stbuf), 0)) return 0;
	}

	return 0;
}

int Vpk::Fuse::open(const char *path, struct fuse_file_info *fi) {
	Descrs::iterator di = m_descrs.find(path);

	if (di != m_descrs.end()) {
		fi->fh = di->second;
		return 0;
	}

	Node *node = m_package.get(path);
	
	if (!node)
		return -ENOENT;

	if (node->type() == Node::DIR)
		return -EISDIR;

	if((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	// find free file descriptor
	uint64_t max = std::numeric_limits<uint64_t>::max();
	for (uint64_t fd = 1;; ++ fd) {
		if (m_filemap.find(fd) == m_filemap.end()) {
			m_descrs[path] = fi->fh = fd;
			m_filemap[fd] = std::pair<std::string,File*>(path, (File*) node);
			break;
		}

		if (fd == max)
			return -ENFILE;
	}

	return 0;
}

boost::filesystem::path Vpk::Fuse::archivePath(uint16_t index) const {
	return fs::path(m_package.srcdir()) /
		(boost::format("%s_%03d.vpk") % m_package.name() % index).str();
}

boost::shared_ptr<boost::filesystem::ifstream> Vpk::Fuse::archive(uint16_t index) {
	Archives::iterator i = m_archives.find(index);
	if (i != m_archives.end()) {
		return i->second;
	}
	else {
		fs::path archivePath(this->archivePath(index));

		if (!fs::exists(archivePath)) {
			std::cerr << "*** archive does not exist: " << archivePath << std::endl;
			return boost::shared_ptr<boost::filesystem::ifstream>();
		}
		boost::shared_ptr<boost::filesystem::ifstream> archive(
			new fs::ifstream(archivePath, std::ios::in | std::ios::binary));
		return m_archives[index] = archive;
	}
}

int Vpk::Fuse::read(const char *path, char *buf, size_t size, off_t offset,
         struct fuse_file_info *fi) {
	Filemap::iterator i = m_filemap.find(fi->fh);

	if (i == m_filemap.end()) {
		return -EBADF;
	}

	File *file = i->second.second;

	if (file->size) {
		if (offset >= file->size) {
			return 0;
		}
		boost::shared_ptr<fs::ifstream> archive = this->archive(file->index);

		if (!archive)
			return 0;

		size_t n = std::min(size, (size_t)(file->size - offset));
		archive->seekg(file->offset + offset);
		archive->read(buf, n);
		n = archive->gcount();

		if (archive->bad()) {
			archive->close();
			m_archives.erase(file->index);
		}
		
		return n;
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

int Vpk::Fuse::release(const char *path, struct fuse_file_info *fi) {
	Filemap::iterator i = m_filemap.find(fi->fh);

	if (i == m_filemap.end()) {
		return -EBADF;
	}

	m_descrs.erase(i->second.first);
	m_filemap.erase(fi->fh);

	return 0;
}
