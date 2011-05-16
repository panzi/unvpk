UNVPK
=====
Unvpk is a small command line tool for listing, checking and extracting of VPK
archives. Such archives are used in Steam engine based games like Portal 2.

Usage
-----
	unvpk COMMAND [OPTION...] ARCHIVE [FILE...]
	
	ARCHIVE has to be a file named "*_dir.vpk".
	If one or more FILEs are given only these are listed/checked/extracted.
	
	Commands:
	    l                   list archive contents
	    x                   extract archive
	    c                   check archive
	
	Options:
	  --help                print help message
	  --version             print version information
	  --check               check CRC32 sums during extraction
	  --stop                stop on error

Dependencies
------------
This program uses following components of the [boost][1] C++ library:

 * `system`
 * `filesystem`
 * `program_options`

Notes
-----
This program is written by Mathias Panzenböck and released under the LGPL v2.1.
However, I looked at the source of [Gibbeds VPK Extractor][2] ([SVN][3]) in
order to find out how VPK works.

[1]: http://www.boost.org/
[2]: http://blog.gib.me/2009/07/07/left4dead-vpk-extraction-tools-updated/
[3]: http://svn.gib.me/public/valve/trunk/
