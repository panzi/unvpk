#include <string>
#include <iostream>
#include <exception>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include <vpk.h>

namespace fs = boost::filesystem;

void usage() {
	std::cout <<
		"Usage: unvpk COMMAND ARCHIVE [FILES]\n"
		"TODO: Description.\n";
}

int main(int argc, char *argv[]) {
	if (argc < 2)  {
		usage();
		return 0;
	}

	std::string command = argv[1];
	std::string archive = "-";
	std::vector<std::string> filter;

	if (argc > 2) {
		archive = argv[2];
		std::copy(argv+3, argv+argc, std::back_inserter(filter));
	}

	if (command != "l" && command != "x" && command != "c") {
		std::cerr << "unkonwn command: " << command << std::endl;
		usage();
		return 1;
	}

	Vpk::ConsoleHandler handler;
	Vpk::Package package(&handler);

	try {
		if (archive == "-") {
			std::cin.exceptions(std::ios::failbit | std::ios::badbit);
			package.read(".", "", std::cin);
		}
		else {
			package.read(archive);
		}

		if (!filter.empty()) {
			package.filter(filter);
		}

		if (command == "l") {
			package.list();
		}
		else if (command == "x") {
			package.extract(".");
		}
		else if (command == "c") {
			package.check();
		}
	}
	catch (const std::exception &exc) {
		std::cerr << "*** error: " << exc.what() << std::endl;
		return 1;
	}

	return handler.allok() ? 0 : 1;
}
