VPKFS
=====
VPKFS is a small FUSE based filesystem which allows you to mount VPK archives.

Usage
-----
	Usage: vpkfs [OPTIONS] ARCHIVE MOUNTPOINT
	
	ARCHIVE has to be a file named "*_dir.vpk".
	This filesystem is read-only, single threaded and only supports blocking operations.
	
	Options:
		-o opt,[opt...]        mount options
		-h   --help            print help
		-v   --version         print version
		-d   -o debug          enable debug output (implies -f)
		-f                     foreground operation