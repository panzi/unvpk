#include <iostream>

#include <vpk/console_handler.h>

void Vpk::ConsoleHandler::end() {
	std::cout << "successful " << m_success << ", failed " << m_fail << std::endl;
}

void Vpk::ConsoleHandler::direrror(const std::exception &exc) {
	if (m_raise) {
		throw exc;
	}
	else {
		println(std::string("ERROR: ") + exc.what());
	}
}

void Vpk::ConsoleHandler::fileerror(const std::exception &exc) {
	++ m_fail;
	if (m_raise) {
		throw exc;
	}
	else {
		std::cout << ", ERROR: " << exc.what() << std::endl;
	}
}

bool Vpk::ConsoleHandler::filterfile(const std::string &path) {
	if (super_type::filterfile(path)) {
		print(path);
		return true;
	}
	else {
		return false;
	}
}

void Vpk::ConsoleHandler::print(const std::string &msg) {
	std::cout << (boost::format("[ %3lf%% ] %s") % progress() % msg).str();
}
