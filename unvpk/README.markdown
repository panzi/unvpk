UNVPK
=====
Unvpk is a small command line tool for listing, checking and extracting of VPK
archives. Such archives are used in Source engine based games like Portal 2.

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
