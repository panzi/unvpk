VPK File Format Specification
=============================
VPK stores files into several archives. The directory information is stored
in a file that ends with "`_dir.vpk`". All other archives related to this
package are located in the same directory and have the same prefix but instead
of "`_dir.vpk`" they end in "`_###.vpk`", where "`###`" is a zero padded number.

Example directory listing:

	pak01_000.vpk
	pak01_001.vpk
	pak01_002.vpk
	pak01_dir.vpk

The Directory File
------------------
Note that all numbers in VPK are stored in *little endian* byte order.
In this document `AsciiZ` denotes a zero-terminated ASCII string.

The seemingly complete format of the directory file:

	// optional header
	{
		U32 magic = 0x55AA1234
		U32 vpk_version // only known version so far is 1
		U32 index_size
	} || {}

	// directory structure
	{

		AsciiZ filetype
		{

			AsciiZ pathname // sub-directories are separated with "/"
			{

				AsciiZ filename
				U32    CRC32
				U16    inlined_file_size
				U16    archive_index // the "###" part of the archive files
				U32    offset
				U32    file_size // if 0x0 then the data is inlined and not
				                 // stored in anther file
				U16    file_terminator = 0xFFFF
				U8[inlined_file_size] inlined_data

			// an empty filename denotes the end of the list:
			} || U8 file_list_terminator = 0x0

		// an empty path denotes the end of the list:
		} || U8 path_list_terminator = 0x0

	// an empty filetype denotes the end of the list:
	} || U8 filetype_list_terminator = 0x0

File paths for archive extraction can then be reconstructed as:

	pathname + "/" + filename + "." + filetype

The archive files seem not to contain any data besides the files.

Oddities
--------
Even though the format specification above seems to be complete a few oddities
are encountered in the wild:

 * The CRC32 sums do not match in rare cases, but Steam does not detect any
   error when checking the archive.
 * There are sometimes areas in the archive files that are not referenced by
   any file. These areas seem to contain valid files (I've seen WAV, VTX and
   VCD at a glance). Still, Steam does not report any error for this either.

Also in theory a file could contain inlined data and referenced data at the
same time. However, I haven't seen such a directory file in the wild.
