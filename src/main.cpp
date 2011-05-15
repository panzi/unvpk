#include <string>
#include <iostream>
#include <algorithm>
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

std::string tolower(const std::string &s) {
	std::string s2(s);
	std::transform(s2.begin(), s2.end(), s2.begin(), (int (*)(int)) std::tolower);
	return s2;
}

std::string &tolower(std::string &s) {
	std::transform(s.begin(), s.end(), s.begin(), (int (*)(int)) std::tolower);
	return s;
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

	std::string srcdir(".");
	std::string name("");
	if (archive != "-") {
		fs::path archivepath(archive);
		std::string leaf(archivepath.filename());
		srcdir = archivepath.parent_path().string();
		
		if (leaf.size() < 8 || tolower(leaf.substr(leaf.size()-8)) != "_dir.vpk") {
			std::cerr << "*** error: file does not end in \"_dir.vpk\": " << archive;
			return 1;
		}
		name = leaf.substr(0, leaf.size()-8);
	}

	Vpk::Package package(name, srcdir);
	try {
		if (archive == "-") {
			std::cin.exceptions(std::ios::failbit | std::ios::badbit);
			package.read(std::cin);
		}
		else {
			fs::ifstream is;
			is.exceptions(std::ios::failbit | std::ios::badbit);
			is.open(archive, std::ios::in | std::ios::binary);
			package.read(is);
		}

		if (command == "l") {
			package.list(filter);
		}
		else if (command == "x") {
			if (!package.extract(".")) {
				std::cerr << "error extracting vpk package\n";
			}
		}
		else if (command == "c") {
			std::cerr << "CRC32 validation not implemented yet\n";
			return 1;
		}
	}
	catch (const std::exception &exc) {
		std::cerr << "*** error: " << exc.what() << std::endl;
		return 1;
	}

	return 0;
}
