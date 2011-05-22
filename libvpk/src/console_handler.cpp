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
#include <iostream>

#include <boost/format.hpp>

#include <vpk/console_handler.h>

void Vpk::ConsoleHandler::begin(const Package &package) {
	m_begun      = true;
	m_extracting = false;
	m_filecount  = package.filecount();
	m_success = 0;
	m_fail    = 0;
}

void Vpk::ConsoleHandler::end() {
	m_begun      = false;
	m_extracting = false;
	println(boost::format("%d successful, %d failed") % m_success % m_fail);
}

bool Vpk::ConsoleHandler::direrror(const std::exception &exc, const std::string &path) {
	if (m_extracting) {
		++ m_fail;
		m_extracting = false;
		std::cout << std::endl;
	}

	if (!m_raise) {
		println(boost::format("*** error creating directory \"%s\": %s") % path % exc.what());
	}
	return m_raise;
}

bool Vpk::ConsoleHandler::fileerror(const std::exception &exc, const std::string&) {
	if (m_extracting) {
		++ m_fail;
		m_extracting = false;
	}

	if (m_raise) {
		std::cout << std::endl;
	}
	else {
		std::cout << ", error: " << exc.what() << std::endl;
	}
	return m_raise;
}

bool Vpk::ConsoleHandler::archiveerror(const std::exception &exc, const std::string &path) {
	if (m_extracting) {
		++ m_fail;
		m_extracting = false;
		std::cout << std::endl;
	}

	if (!m_raise) {
		println(boost::format("*** error reading archive \"%s\": %s") % path % exc.what());
	}
	return m_raise;
}

bool Vpk::ConsoleHandler::filtererror(const std::exception &exc, const std::string &path) {
	if (m_extracting) {
		++ m_fail;
		m_extracting = false;
		std::cout << std::endl;
	}

	if (!m_raise) {
		std::cout << "*** error reading entry \"" << path << "\": " << exc.what() << std::endl;
	}
	return m_raise;
}

void Vpk::ConsoleHandler::extract(const std::string &filepath) {
	m_extracting = true;
	print(filepath);
}

void Vpk::ConsoleHandler::success(const std::string&) {
	m_extracting = false;
	++ m_success;
	std::cout << std::endl;
}

void Vpk::ConsoleHandler::print(const std::string &msg) {
	if (m_begun) {
		std::cout << (boost::format("[ %3.0lf%% ] %s") % (100 * progress()) % msg).str() << std::flush;
	}
	else {
		std::cout << msg << std::flush;
	}
}
