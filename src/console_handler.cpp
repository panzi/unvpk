#include <iostream>

#include <boost/format.hpp>

#include <vpk/console_handler.h>

void Vpk::ConsoleHandler::end() {
	std::cout << "successful " << m_success << ", failed " << m_fail << std::endl;
}

void Vpk::ConsoleHandler::direrror(const std::exception &exc, const std::string &path) {
	if (m_raise) {
		throw exc;
	}
	else {
		println(boost::format("*** error creating directory \"%s\": %s") % path % exc.what());
	}
}

void Vpk::ConsoleHandler::fileerror(const std::exception &exc, const std::string &path) {
	m_extracting = false;
	++ m_fail;
	if (m_raise) {
		throw exc;
	}
	else {
		std::cout << ", error: " << exc.what() << std::endl;
	}
}

void Vpk::ConsoleHandler::archiveerror(const std::exception &exc, const std::string &path) {
	if (m_raise) {
		throw exc;
	}
	else {
		if (m_extracting) std::cout << std::endl;
		println(boost::format("*** error reading archive \"%s\": %s") % path % exc.what());
	}
}

void Vpk::ConsoleHandler::extract(const std::string &filepath) {
	m_extracting = true;
	print(filepath);
}

void Vpk::ConsoleHandler::success(const std::string &filepath) {
	m_extracting = false;
	++ m_success;
	std::cout << std::endl;
}

void Vpk::ConsoleHandler::print(const std::string &msg) {
	std::cout << (boost::format("[ %3lf%% ] %s") % progress() % msg).str();
}
