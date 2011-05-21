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
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <vpk.h>
#include <vpk/console_handler.h>
#include <vpk/console_table.h>

namespace fs   = boost::filesystem;
namespace po   = boost::program_options;
namespace algo = boost::algorithm;

using boost::lexical_cast;

static void usage(const po::options_description &desc) {
	std::cout <<
		"Usage: unvpk [OPTION...] ARCHIVE [FILE...]\n"
		"List, check and extract VPK archives.\n"
		"ARCHIVE has to be a file named \"*_dir.vpk\".\n"
		"If one or more FILEs are given only these are listed/checked/extracted.\n"
		"\n" <<
		desc <<
		"\n"
		"(c) 2011 Mathias Panzenböck\n";
}

template<typename SizeFormatter>
static void list(
		const Vpk::Nodes &nodes,
		const std::vector<std::string> &prefix,
		Vpk::ConsoleTable &table,
		SizeFormatter szfmt,
		size_t &files,
		size_t &dirs,
		size_t &sumsize) {
	for (Vpk::Nodes::const_iterator it = nodes.begin(); it != nodes.end(); ++ it) {
		Vpk::Node *node = it->second.get();
		std::vector<std::string> path(prefix);
		path.push_back(node->name());
		if (node->type() == Vpk::Node::DIR) {
			list(((const Vpk::Dir*) node)->nodes(), path, table, szfmt, files, dirs, sumsize);
			++ dirs;
		}
		else {
			Vpk::File *file = (Vpk::File *) node;
			size_t size = file->size;
			
			if (size) {
				table.row(file->index,
					boost::format("%08x") % file->crc32, szfmt(size), algo::join(path, "/"));
			}
			else {
				size = file->data.size();
				table.row("-",
					boost::format("%08x") % file->crc32, szfmt(size), algo::join(path, "/"));
			}

			sumsize += size;
			++ files;
		}
	}
}

static size_t bytes(size_t size) {
	return size;
}

static std::string humanReadableSize(size_t size) {
	if (size < 1024) {
		return lexical_cast<std::string>(size);
	}
	else if (size < 1024L * 1024) {
		return (boost::format("%.1lfK") % (size / (double)1024)).str();
	}
	else if (size < 1024L * 1024 * 1024) {
		return (boost::format("%.1lfM") % (size / (double)(1024L * 1024))).str();
	}
	else if (size < 1024LL * 1024 * 1024 * 1024) {
		return (boost::format("%.1lfG") % (size / (double)(1024L * 1024 * 1024))).str();
	}
	else if (size < 1024LL * 1024 * 1024 * 1024 * 1024) {
		return (boost::format("%.1lfT") % (size / (double)(1024LL * 1024 * 1024 * 1024))).str();
	}
	else if (size < 1024LL * 1024 * 1024 * 1024 * 1024 * 1024) {
		return (boost::format("%.1lfP") % (size / (double)(1024LL * 1024 * 1024 * 1024 * 1024))).str();
	}
	else {
		return (boost::format("%.1lfE") % (size / (double)(1024LL * 1024 * 1024 * 1024 * 1024 * 1024))).str();
	}
}

static void list(const Vpk::Package &package, bool humanreadable) {
	Vpk::ConsoleTable table;
	table.columns(Vpk::ConsoleTable::RIGHT, Vpk::ConsoleTable::RIGHT, Vpk::ConsoleTable::RIGHT, Vpk::ConsoleTable::LEFT);
	table.row("Archive", "CRC32", "Size", "Filename");
	size_t files = 0, dirs = 0, sumsize = 0;
	if (humanreadable) {
		list(package.nodes(), std::vector<std::string>(), table, humanReadableSize, files, dirs, sumsize);
	}
	else {
		list(package.nodes(), std::vector<std::string>(), table, bytes, files, dirs, sumsize);
	}
	table.print(std::cout);
	std::cout << files << " "<< (files == 1 ? "file" : "files") << " (";
	if (humanreadable) {
		std::cout << humanReadableSize(sumsize);
	}
	else {
		std::cout << sumsize;
	}
	std::cout << " total size), " << dirs << " " << (dirs == 1 ? "directory" : "directories") << "\n";
}

int main(int argc, char *argv[]) {
	po::options_description desc("Options");
	desc.add_options()
		("help,H",           "print help message")
		("version,v",        "print version information")
		("list,l",           "list archive contents")
		("human-readable,h", "use human readable file sizes in listing")
		("check,c",          "check CRC32 sums")
		("xcheck,x",         "extract and check CRC32 sums")
		("directory,C",      po::value<std::string>(), "extract files into another directory")
		("stop,s",           "stop on error");

	po::options_description hidden;
	hidden.add_options()
		("archive", po::value<std::string>(), "vpk archive")
		("filter",  po::value< std::vector<std::string> >(), "files to process");

	po::options_description opts;
	opts.add(desc).add(hidden);

	po::positional_options_description pos;
	pos.add("archive", 1);
	pos.add("filter", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(opts).positional(pos).run(), vm);
	po::notify(vm);    

	if (vm.count("help") || vm.count("archive") < 1) {
		usage(desc);
		return 0;
	}
	else if (vm.count("version")) {
		std::cout << "unvpk version " << Vpk::VERSION << std::endl;
		return 0;
	}

	bool list          = vm.count("list")   > 0;
	bool check         = vm.count("check")  > 0;
	bool xcheck        = vm.count("xcheck") > 0;
	bool stop          = vm.count("stop")   > 0;
	bool humanreadable = vm.count("human-readable")   > 0;

	std::string directory = vm.count("directory") > 0 ? vm["directory"].as<std::string>() : std::string(".");
	std::string archive   = vm.count("archive")   > 0 ? vm["archive"].as<std::string>()   : std::string("-");
	std::vector<std::string> filter;
	
	if (vm.count("filter") > 0) {
		filter = vm["filter"].as< std::vector<std::string> >();
	}

	Vpk::ConsoleHandler handler(stop);
	Vpk::Package package(&handler);

	try {
		if (archive == "-") {
			Vpk::FileIO io(stdin);
			package.read(".", "", io);
		}
		else {
			package.read(archive);
		}

		if (!filter.empty()) {
			package.filter(filter);
		}

		if (list) {
			::list(package, humanreadable);
		}
		else if (xcheck) {
			package.extract(directory, true);
		}
		else if (check) {
			package.check();
		}
		else {
			package.extract(directory, false);
		}
	}
	catch (const std::exception &exc) {
		std::cerr << "*** error: " << exc.what() << std::endl;
		return 1;
	}

	return handler.allok() ? 0 : 1;
}
