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
	if (m_extracting) std::cout << std::endl;

	if (!m_raise) {
		println(boost::format("*** error creating directory \"%s\": %s") % path % exc.what());
	}
	return m_raise;
}

bool Vpk::ConsoleHandler::fileerror(const std::exception &exc, const std::string &path) {
	m_extracting = false;
	++ m_fail;
	if (m_raise) {
		std::cout << std::endl;
	}
	else {
		std::cout << ", error: " << exc.what() << std::endl;
	}
	return m_raise;
}

bool Vpk::ConsoleHandler::archiveerror(const std::exception &exc, const std::string &path) {
	if (m_extracting) std::cout << std::endl;

	if (!m_raise) {
		println(boost::format("*** error reading archive \"%s\": %s") % path % exc.what());
	}
	return m_raise;
}

bool Vpk::ConsoleHandler::filtererror(const std::exception &exc, const std::string &path) {
	if (m_extracting) std::cout << std::endl;

	if (!m_raise) {
		std::cout << "*** error reading entry \"" << path << "\": " << exc.what() << std::endl;
	}
	return m_raise;
}

void Vpk::ConsoleHandler::extract(const std::string &filepath) {
	if (m_extracting) {
		std::cout << std::endl;
	}
	else {
		m_extracting = true;
	}
	print(filepath);
}

void Vpk::ConsoleHandler::success(const std::string &filepath) {
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
