UNVPK
=====
This project implements a VPK reading facilities. See the subfolder
unvpk for a simple extraction tool and the subfolder vpkfs for a
[FUSE][1] based filesystem that allows you to mount VPK archives.

VPK archives are used in Valve Source engine based games like Portal 2.

Unvpk
-----
Unvpk is a small command line tool for listing, checking and extracting of VPK
archives. Such archives are used in Source engine based games like Portal 2.

### Usage
	unvpk [OPTION...] ARCHIVE [FILE...]
	
	ARCHIVE has to be a file named "*_dir.vpk".
	If one or more FILEs are given only these are listed/checked/extracted.
	
	Options:
	  -H [ --help ]            print help message
	  -v [ --version ]         print version information
	  -l [ --list ]            list archive contents
	  -h [ --human-readable ]  use human readable file sizes in listing
	  -c [ --check ]           check CRC32 sums
	  -x [ --xcheck ]          extract and check CRC32 sums
	  -C [ --directory ] arg   extract files into another directory
	  -s [ --stop ]            stop on error

Vpkfs
-----
Vpkfs is a small FUSE based filesystem which allows you to mount VPK archives.

### Usage
	vpkfs [OPTIONS] ARCHIVE MOUNTPOINT
	
	ARCHIVE has to be a file named "*_dir.vpk".
	This filesystem is read-only and only supports blocking operations.
	
	Options:
		-o opt,[opt...]        mount options
		-h   --help            print help
		-v   --version         print version
		-d   -o debug          enable debug output (implies -f)
		-f                     foreground operation
	    -s                     disable multi-threaded operation
	                           Note that this might actually increase
	                           performance in case you access the
	                           filesystem with only one process.

Setup
-----
### Download
	wget https://bitbucket.org/panzi/unvpk/get/tip.tar.bz2 -O unvpk.tar.bz2
	tar xvjf unvpk.tar.bz2
	cd repos*

### Or Clone
	hg clone https://bitbucket.org/panzi/unvpk
	cd unvpk

### Build and Install
	mkdir build
	cd build
	cmake -DCMAKE_INSTALL_PREFIX=/usr ..
	make -j2 && sudo make install

If you don't want to build and install unvpk replace the cmake line with:
	cmake -DCMAKE_INSTALL_PREFIX=/usr WITH_UNVPK=OFF ..

If you don't want to build and install vpkfs replace the cmake line with:
	cmake -DCMAKE_INSTALL_PREFIX=/usr WITH_VPKFS=OFF ..

Dependencies
------------
This project uses following components of the [Boost][2] C++ library:

 * `system`
 * `filesystem`
 * `program_options`

For vpkfs FUSE is needed.

Notes
-----
This project is written by Mathias Panzenböck and released under the LGPL v2.1.
However, I looked at the source of [Gibbeds VPK Extractor][3] ([SVN][4]) in
order to find out how VPK works.

[1]: http://fuse.sourceforge.net/
[2]: http://www.boost.org/
[3]: http://blog.gib.me/2009/07/07/left4dead-vpk-extraction-tools-updated/
[4]: http://svn.gib.me/public/valve/trunk/
