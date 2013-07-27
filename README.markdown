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
	  -S [ --sort ] arg        sort listing by a comma separated list of keys:
	                               a, archive    archive index
	                               c, crc32      CRC32 checksum
	                               o, offset     offset in archive
	                               s, size       file size
	                               n, name       file name
	                           prepend - to the key to indicate descending sort 
	                           order
	  -h [ --human-readable ]  use human readable file sizes in listing
	  -c [ --check ]           check CRC32 sums
	  -x [ --xcheck ]          extract and check CRC32 sums
	  -C [ --directory ] arg   extract files into another directory
	  -s [ --stop ]            stop on error
	  --coverage               coverage analysis of archive data (archive 
	                           debugging)
	  --dump-uncovered         dump uncovered areas into files (implies --coverage,
	                           archive debugging)

Vpkfs
-----
Vpkfs is a small FUSE based filesystem which allows you to mount VPK archives.

### Usage
	vpkfs [OPTIONS] ARCHIVE MOUNTPOINT
	
	ARCHIVE has to be a file named "*_dir.vpk".
	This filesystem is read-only and only supports blocking operations.
	
	Options:
		-o opt,[opt...]        mount options (see: man fuse)
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

For vpkfs [FUSE][1] is needed.

File Format
-----------
This is a short description of the Valve Packet file format (VPK).

### `*_dir.vpk`
To my knowlege there are currently (as of Portal 2) two versions of the VPK
file format. The only difference is that one has a file magic, version
information and an explicitely given index size, and the other (older) don't.

All values are stored in **little endian**. Offset and Size are given in
bytes.

#### Types

	Byte ..... a single byte
	U32 ...... unsigned 32-bit integer
	ASCIIZ ... zero terminated ASCII string

#### File header (optional)
If the file doesn't start wit the file magic `0x55AA1234` then the index
table directly starts at the beginning of the file.

	Offset  Size  Type  Description
	     0     4  U32   file magic: 0x55AA1234
	     4     4  U32   version: 1
	     8     4  U32   index size

#### Index
Files are grouped by their type (file name extension).
	
	Offset  Size  Type    Description
	[
	     0     N  ASCIIZ  File type
		 
	]*
	           1  Byte    List-Terminator: 0x00



### `*_###.vpk`
These are simple archives where the contained files are simply merged together
in one big file. You can think of them being created like this:

	cat file1 file2 file3 > pak01_001.vpk

Notes
-----
This project is written by Mathias Panzenböck and released under the LGPL v2.1.
However, I looked at the source of [Gibbeds VPK Extractor][3] ([SVN][4]) in
order to find out how VPK works.

[1]: http://fuse.sourceforge.net/
[2]: http://www.boost.org/
[3]: http://blog.gib.me/2009/07/07/left4dead-vpk-extraction-tools-updated/
[4]: http://svn.gib.me/public/valve/trunk/
