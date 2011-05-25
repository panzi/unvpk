/**
 * unvpk - list, check and extract vpk archives
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
#include <algorithm>

#include <vpk/magic.h>
#include <vpk/simple_magic.h>
#include <vpk/multipart_magic.h>

static Vpk::Magics buildMagics();

static Vpk::Magics magics = buildMagics();

size_t Vpk::Magic::maxSize() {
	size_t size = 0;
	for (Magics::const_iterator i = magics.begin(); i != magics.end(); ++ i) {
		size_t sz = (*i)->size();
		if (sz > size) {
			size = sz;
		}
	}
	return size;
}

std::string Vpk::Magic::extensionOf(const char buf[], size_t size) {
	for (Magics::const_iterator i = magics.begin(); i != magics.end(); ++ i) {
		MagicPtr magic = *i;
		if (magic->matches(buf, size)) {
			return magic->type();
		}
	}
	return "bin";
}

void Vpk::Magic::add(MagicPtr magic) {
	magics.push_back(magic);
}

void Vpk::Magic::remove(MagicPtr magic) {
	Magics::iterator i = std::find(magics.begin(), magics.end(), magic);
	if (i != magics.end()) {
		magics.erase(i);
	}
}

static Vpk::Magics buildMagics() {
	Vpk::Magics magics;

	Vpk::MultipartMagic *mult = new Vpk::MultipartMagic("wav");
	mult->put(0, "RIFF");
	mult->put(8, "WAVEfmt");
	magics.push_back(Vpk::MagicPtr(mult));
	magics.push_back(Vpk::MagicPtr(new Vpk::SimpleMagic("vtx", "VTX")));
	magics.push_back(Vpk::MagicPtr(new Vpk::SimpleMagic("vtf", "VTF")));
	magics.push_back(Vpk::MagicPtr(new Vpk::SimpleMagic("mdl", "IDST")));
	magics.push_back(Vpk::MagicPtr(new Vpk::SimpleMagic("vvd", "IDSV")));
	magics.push_back(Vpk::MagicPtr(new Vpk::SimpleMagic("ani", "IDAG")));
	magics.push_back(Vpk::MagicPtr(new Vpk::SimpleMagic("pcf", "<!-- dmx encoding binary 2 format pcf ")));
	magics.push_back(Vpk::MagicPtr(new Vpk::SimpleMagic("mp3", "ID3")));

	mult = new Vpk::MultipartMagic("phy");
	const char magic[] = {0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	mult->put(0, magic, sizeof(magic));
	mult->put(20, "VPHY");
	magics.push_back(Vpk::MagicPtr(mult));
		
	// missing: indistnguishable plain text formats: res, vcd, vmt
	return magics;
}
