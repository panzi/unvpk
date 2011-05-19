UNVPK
=====
Unvpk is a small command line tool for listing, checking and extracting of VPK
archives. Such archives are used in Source engine based games like Portal 2.

Usage
-----
	unvpk COMMAND [OPTION...] ARCHIVE [FILE...]
	
	ARCHIVE has to be a file named "*_dir.vpk".
	If one or more FILEs are given only these are listed/checked/extracted.
	
	Options:
	  -h [ --help ]          print help message
	  -v [ --version ]       print version information
	  -l [ --list ]          list archive contents
	  -c [ --check ]         check CRC32 sums
	  -x [ --xcheck ]        extract and check CRC32 sums
	  -C [ --directory ] arg extract files into another directory
	  -s [ --stop ]          stop on error
