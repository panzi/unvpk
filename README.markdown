UNVPK
=====
This project implements a VPK reading facilities. See the subfolder
[unvpk](unvpk) for a simple extraction tool and the subfolder [vpkfs](vpkfs) for
a [FUSE][1] based filesystem that allows you to mount VPK archives.

VPK archives are used in Valve Source engine based games like Portal 2.

Dependencies
------------
This program uses following components of the [Boost][2] C++ library:

 * `system`
 * `filesystem`
 * `program_options`

Notes
-----
This program is written by Mathias Panzenböck and released under the LGPL v2.1.
However, I looked at the source of [Gibbeds VPK Extractor][3] ([SVN][4]) in
order to find out how VPK works.

[1]: http://fuse.sourceforge.net/
[2]: http://www.boost.org/
[3]: http://blog.gib.me/2009/07/07/left4dead-vpk-extraction-tools-updated/
[4]: http://svn.gib.me/public/valve/trunk/
