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
#include <string>
#include <iostream>
#include <exception>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/program_options.hpp>

#include <vpk.h>

namespace fs = boost::filesystem;
namespace po = boost::program_options;

void usage(const po::options_description &desc) {
	std::cout <<
		"Usage: unvpk COMMAND [OPTION...] ARCHIVE [FILE...]\n"
		"List, check and extract VPK archives.\n"
		"If one or more FILEs are given only these are listed/checked/extracted.\n"
		"\n"
		"Commands:\n"
		"    l                   list archive contents\n"
		"    x                   extract archive\n"
		"    c                   check archive\n"
		"\n" <<
		desc <<
		"\n"
		"(c) 2011 Mathias Panzenböck\n";
}

int main(int argc, char *argv[]) {
	po::options_description desc("Options");
	desc.add_options()
		("help",    "print help message")
		("version", "print version information")
		("check",   "check CRC32 sums during extraction")
		("stop",    "stop on error");

	po::options_description hidden;
	hidden.add_options()
		("command", po::value<std::string>(), "unvpk command")
		("archive", po::value<std::string>(), "vpk archive")
		("filter",  po::value< std::vector<std::string> >(), "files to process");

	po::options_description opts;
	opts.add(desc).add(hidden);

	po::positional_options_description pos;
	pos.add("command", 1);
	pos.add("archive", 1);
	pos.add("filter", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(opts).positional(pos).run(), vm);
	po::notify(vm);    

	if (vm.count("help") || vm.count("command") < 1) {
		usage(desc);
		return 0;
	}
	else if (vm.count("version")) {
		std::cout << "unvpk version " << Vpk::VERSION << std::endl;
		return 0;
	}

	bool stop = vm.count("stop") > 0;
	bool check = vm.count("check") > 0;

	std::string command = vm["command"].as<std::string>();
	std::string archive = vm.count("archive") > 0 ? vm["archive"].as<std::string>() : std::string("-");
	std::vector<std::string> filter;
	
	if (vm.count("filter") > 0) {
		filter = vm["filter"].as< std::vector<std::string> >();
	}

	if (command != "l" && command != "x" && command != "c") {
		std::cerr << "*** error: unknown command: " << command << std::endl;
		usage(desc);
		return 1;
	}

	Vpk::ConsoleHandler handler(stop);
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
			package.extract(".", check);
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
