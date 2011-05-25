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

#include <boost/algorithm/string.hpp>

#include <vpk/util.h>

namespace fs = boost::filesystem;

static void mkpath(const fs::path &path) {
	fs::path parent = path.parent_path();
	if (!fs::exists(parent)) {
		mkpath(parent);
	}
	fs::create_directory(path);
}

void Vpk::create_path(const fs::path &path) {
	mkpath(fs::system_complete(path));
}

std::string Vpk::tolower(const std::string &s) {
	std::string s2(s);
	std::transform(s2.begin(), s2.end(), s2.begin(), (int (*)(int)) std::tolower);
	return s2;
}

std::string &Vpk::tolower(std::string &s) {
	std::transform(s.begin(), s.end(), s.begin(), (int (*)(int)) std::tolower);
	return s;
}
