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
	:::text
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
	  --stats                  print some statistics and coverage analysis of 
	                           archive data (archive debugging)
	  -a [ --all ]             also show archives with 100% coverage in statistics
	  --dump-uncovered         dump uncovered areas into files (implies --stats, 
	                           archive debugging)

Vpkfs
-----
Vpkfs is a small FUSE based filesystem which allows you to mount VPK archives.

### Usage
	:::text
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
	:::text
	wget https://bitbucket.org/panzi/unvpk/get/tip.tar.bz2 -O unvpk.tar.bz2
	tar xvjf unvpk.tar.bz2
	cd repos*

### Or Clone
	:::text
	hg clone https://bitbucket.org/panzi/unvpk
	cd unvpk

### Build and Install
	:::text
	mkdir build
	cd build
	cmake -DCMAKE_INSTALL_PREFIX=/usr ..
	make -j2 && sudo make install

If you don't want to build and install unvpk replace the cmake line with:

	:::text
	cmake -DCMAKE_INSTALL_PREFIX=/usr -DWITH_UNVPK=OFF ..

If you don't want to build and install vpkfs replace the cmake line with:

	:::text
	cmake -DCMAKE_INSTALL_PREFIX=/usr -DWITH_VPKFS=OFF ..

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

	:::text
	┌─────────────────────────────────────┐
	│                                     │
	│ Header (Optional)                   │
	│                                     │
	│   Magic (0x55AA1234)                │
	│   Version (1)                       │
	│   Index Size                        │
	│                                     │
	├─────────────────────────────────────┤
	│                                     │
	│ Index                               │
	│                                     │
	│ ┌─────────────────────────────────┐ │
	│ │                                 │ │
	│ │ File Type                       │ │
	│ │                                 │ │
	│ │   Type Name                     │ │
	│ │                                 │ │
	│ │ ┌─────────────────────────────┐ │ │
	│ │ │                             │ │ │
	│ │ │ Directory                   │ │ │
	│ │ │                             │ │ │
	│ │ │   Directory Path            │ │ │
	│ │ │                             │ │ │
	│ │ │ ┌─────────────────────────┐ │ │ │
	│ │ │ │                         │ │ │ │
	│ │ │ │ File                    │ │ │ │
	│ │ │ │                         │ │ │ │
	│ │ │ │   File Name             │ │ │ │
	│ │ │ │   CRC32                 │ │ │ │
	│ │ │ │   Preload Size          │ │ │ │
	│ │ │ │   Archive Index         │ │ │ │
	│ │ │ │   Offset                │ │ │ │
	│ │ │ │   Size                  │ │ │ │
	│ │ │ │   Termintator (0xFFFF)  │ │ │ │
	│ │ │ │                         │ │ │ │
	│ │ │ ├─────────────────────────┤ │ │ │
	│ │ │ │                         │ │ │ │
	│ │ │ │ Preload Data            │ │ │ │
	│ │ │ │                         │ │ │ │
	│ │ │ └─────────────────────────┘ │ │ │
	│ │ │                             │ │ │
	│ │ │   ...                       │ │ │
	│ │ │                             │ │ │
	│ │ │   Terminator (0x00)         │ │ │
	│ │ │                             │ │ │
	│ │ └─────────────────────────────┘ │ │
	│ │                                 │ │
	│ │   ...                           │ │
	│ │                                 │ │
	│ │   Terminator (0x00)             │ │
	│ │                                 │ │
	│ └─────────────────────────────────┘ │
	│                                     │
	│   ...                               │
	│                                     │
	│   Terminator (0x00)                 │
	│                                     │
	├─────────────────────────────────────┤
	│                                     │
	│ Data                                │
	│                                     │
	│ ...                                 │
	│                                     │
	└─────────────────────────────────────┘

Version 2 of VPK has more fields in its header (see below) and more after the
data section (a footer?). Details are not known at the moment.

#### Value Types
All values are stored in **little endian**. Offsets and sizes are given in
bytes.

	:::text
	Byte ..... single byte
	Bytes .... byte array of arbitrary data
	U16 ...... unsigned 16-bit integer
	U32 ...... unsigned 32-bit integer
	ASCIIZ ... zero terminated ASCII string

#### Terminators
The terminators of the file type-, directory- and file-lists are simple
zero-bytes. Because the first element of each of those structures are
zero terminated ASCII strings you can write you code so, that when you
read a zero-length name you know the list is terminated. This means that
no file type, directory path or file name may have a zero-length.

#### Header
If the file doesn't start with the file magic `0x55AA1234` then the index
table directly starts at the beginning of the file.

##### Version 1

	Offset  Size  Type  Description
	     0     4  U32   file magic: 0x55AA1234
	     4     4  U32   version: 1
	     8     4  U32   index size

##### Version 2

	Offset  Size  Type  Description
	     0     4  U32   file magic: 0x55AA1234
	     4     4  U32   version: 2
	     8     4  U32   index size
	    12     4  U32   footer offset
	    16     4  U32   always 0?
	    20     4  U32   footer size?
	    24     4  U32   always 48?

#### Index
Files are grouped by their type (file name extension).
	
	Offset  Size  Type    Description
	
	 [ File Type ]...
	
	     ?     1  Byte    terminator: 0x00

#### File Type
	Offset  Size  Type    Description
	     0     N  ASCIIZ  file type (extension like "wav" or "res")
	
	 [ Directory ]...
	
	     ?     1  Byte    terminator: 0x00

#### Directory

	Offset  Size  Type    Description
	     0     N  ASCIIZ  dir path (seperator character: '/')
	
	 [ File ]...
	
	     ?     1  Byte    terminator: 0x00

#### File

	Offset  Size  Type    Description
	     0     N  ASCIIZ  file name (without extension)
	   N+0     4  U32     crc32
	   N+4     2  U16     preload size (PS)
	   N+6     2  U16     archive index, 0x7FFF refers to the index file (*_dir.vpk)
	   N+8     4  U32     offset in archive file
	  N+12     4  U32     size of data in archive file
	  N+16     2  U16     terminator: 0xFFFF
	  N+18    PS  Bytes   preload data

I'm actually not sure if the 0xFFFF terminator is really a terminator
and not just padding or some kind of field that is coincidentally always
0xFFFF in the files I have.

The data of a file consists of the concatenation of the preload data
and the data from the archive file. If the archive index is 0x7FFF the
data is stored in the index file (`*_dir.vpk`). In this case the offset
refers to the offset after the end of the index:

	offset_in_file = header_size + index_size + offset

Note that the `header_size` might be 0.

### `*_###.vpk`
These are simple archives where the contained files are simply merged together
in one big file. You can think of them being created like this (if the preload
size is always 0):

	:::text
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
