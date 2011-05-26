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
#include <map>

#include <boost/filesystem/operations.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>

#include <vpk.h>
#include <vpk/util.h>
#include <vpk/console_handler.h>
#include <vpk/console_table.h>
#include <vpk/coverage.h>
#include <vpk/magic.h>

namespace fs   = boost::filesystem;
namespace po   = boost::program_options;
namespace algo = boost::algorithm;

using boost::lexical_cast;

using namespace Vpk;

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
		const Nodes &nodes,
		const std::vector<std::string> &prefix,
		ConsoleTable &table,
		SizeFormatter szfmt,
		size_t &files,
		size_t &dirs,
		size_t &sumsize) {
	for (Nodes::const_iterator it = nodes.begin(); it != nodes.end(); ++ it) {
		Node *node = it->second.get();
		std::vector<std::string> path(prefix);
		path.push_back(node->name());
		if (node->type() == Node::DIR) {
			list(((const Dir*) node)->nodes(), path, table, szfmt, files, dirs, sumsize);
			++ dirs;
		}
		else {
			File *file = (File *) node;
			size_t size = file->preload.size() + file->size;
			
			if (file->size) {
				table.row(file->index,
					boost::format("%08x") % file->crc32, szfmt(size), algo::join(path, "/"));
			}
			else {
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

static void list(const Package &package, bool humanreadable) {
	ConsoleTable table;
	table.columns(ConsoleTable::RIGHT, ConsoleTable::RIGHT, ConsoleTable::RIGHT, ConsoleTable::LEFT);
	table.row("Archive", "CRC32", "Size", "Filename");
	size_t files = 0, dirs = 0, sumsize = 0;
	if (humanreadable) {
		list(package.nodes(), std::vector<std::string>(), table, Coverage::humanReadableSize, files, dirs, sumsize);
	}
	else {
		list(package.nodes(), std::vector<std::string>(), table, bytes, files, dirs, sumsize);
	}
	table.print(std::cout);
	std::cout << files << " "<< (files == 1 ? "file" : "files") << " (";
	if (humanreadable) {
		std::cout << Coverage::humanReadableSize(sumsize);
	}
	else {
		std::cout << sumsize;
	}
	std::cout << " total size), " << dirs << " " << (dirs == 1 ? "directory" : "directories") << "\n";
}

typedef std::map<int,Coverage> Coverages;

static void coverage(const Dir &dir, Coverages &covs) {
	for (Dir::const_iterator i = dir.begin(); i != dir.end(); ++ i) {
		Node *node = i->second.get();
		if (node->type() == Node::DIR) {
			coverage(*(Dir*) node, covs);
		}
		else {
			File *file = (File*) node;
			if (file->size) {
				covs[file->index].add(file->offset, file->size);
			}
		}
	}
}

static void coverage(
		const fs::path &archindex,
		off_t dirPos,
		const Package &package,
		bool dump,
		const fs::path &destdir,
		bool humanreadable) {
	Coverages covs;
	
	covs[-1].add(0, dirPos);

	std::string prefix = tolower(package.name());
	prefix += '_';
	for (fs::directory_iterator i = fs::directory_iterator(package.srcdir()), end; i != end; ++ i) {
		std::string name = tolower(i->path().filename());

		if (boost::starts_with(name, prefix) && boost::ends_with(name, ".vpk")) {
			std::string digits(name, prefix.size(), name.size() - prefix.size() - 4);
			if (digits.size() >= 3) {
				try {
					uint16_t index = boost::lexical_cast<uint16_t>(digits);
					covs[index];
				}
				catch (const boost::bad_lexical_cast&) {}
			}
		}
	}

	coverage(package, covs);

	if (dump) {
		create_path(destdir);
	}

	size_t uncovered = 0;
	size_t total = 0;
	std::vector<char> magic(Magic::maxSize(), 0);
	for (Coverages::const_iterator i = covs.begin(); i != covs.end(); ++ i) {
		std::string archive;
		size_t size;

		if (i->first == -1) {
			size = fs::file_size(archindex);
			archive = archindex.filename();
		}
		else {
			fs::path path = package.archivePath(i->first);
			size = fs::file_size(path);
			archive = path.filename();
		}

		total += size;
		std::string sizeStr = humanreadable ?
			Coverage::humanReadableSize(size) :
			boost::lexical_cast<std::string>(size);

		const Coverage &covered = i->second;
		Coverage missing = covered.invert(size);
		
		size_t missingSize = missing.coverage();
		
		if (missingSize == 0)
			continue;

		uncovered += missingSize;
		std::string missingSizeStr = humanreadable ?
			Coverage::humanReadableSize(missingSize) :
			boost::lexical_cast<std::string>(missingSize);
		
		size_t coveredSize = covered.coverage();
		std::string coveredSizeStr = humanreadable ?
			Coverage::humanReadableSize(coveredSize) :
			boost::lexical_cast<std::string>(coveredSize);

		std::cout
			<< (boost::format(
				"File: %s\n"
				"Size: %s\n"
				"Covered: %s (%.0lf%%)\n"
				"Missing: %s\n"
				"Missing Areas:\n"
				"\t%s\n\n")
				% archive % sizeStr % coveredSizeStr % (covered.coverage() * (double)100 / size)
				% missingSizeStr % missing.str(humanreadable));

		if (dump) {
			std::string prefix = (destdir / archive).string();
			FileIO arch(fs::path(package.srcdir()) / archive, "rb");

			const Coverage::Slices &slices = missing.slices();
			for (Coverage::Slices::const_iterator i = slices.begin(); i != slices.end(); ++ i) {
				arch.seek(i->first, FileIO::SET);
				size_t count = std::min(i->second, magic.size());
				arch.read(&magic[0], count);

				std::string type = Magic::extensionOf(&magic[0], count);
				std::string filename = (boost::format("%s_%lu_%lu.%s") % prefix % i->first % i->second % type).str();
				std::string sizeStr = humanreadable ?
					Coverage::humanReadableSize(i->second) :
					boost::lexical_cast<std::string>(i->second);
				std::cout << "Dumping " << sizeStr << " to \"" << filename << "\"\n";

				FileIO out(filename, "wb");
				arch.seek(i->first, FileIO::SET);
				arch.read(out, i->second);
			}
			std::cout << std::endl;
		}
	}

	std::string totalStr = humanreadable ?
		Coverage::humanReadableSize(total) :
		boost::lexical_cast<std::string>(total);

	size_t covered = total - uncovered;
	std::string coveredStr = humanreadable ?
		Coverage::humanReadableSize(covered) :
		boost::lexical_cast<std::string>(covered);

	std::string uncoveredStr = humanreadable ?
		Coverage::humanReadableSize(uncovered) :
		boost::lexical_cast<std::string>(uncovered);

	std::cout
		<< (boost::format(
			"Total Size: %s\n"
			"Total Covered: %s (%.0lf%%)\n"
			"Total Missing: %s\n")
		% totalStr % coveredStr % (covered * (double)100 / total)
		% uncoveredStr);
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
		("stop,s",           "stop on error")
		("coverage",         "coverage analysis of archive data (archive debugging)")
		("dump-uncovered",   "dump uncovered areas into files (archive debugging)");

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
		std::cout << "unvpk version " << VERSION << std::endl;
		return 0;
	}

	bool list          = vm.count("list")     > 0;
	bool check         = vm.count("check")    > 0;
	bool xcheck        = vm.count("xcheck")   > 0;
	bool stop          = vm.count("stop")     > 0;
	bool coverage      = vm.count("coverage") > 0;
	bool dump          = vm.count("dump-uncovered") > 0;
	bool humanreadable = vm.count("human-readable") > 0;

	std::string directory = vm.count("directory") > 0 ? vm["directory"].as<std::string>() : std::string(".");
	std::string archive   = vm.count("archive")   > 0 ? vm["archive"].as<std::string>()   : std::string("-");
	std::vector<std::string> filter;
	
	if (vm.count("filter") > 0) {
		filter = vm["filter"].as< std::vector<std::string> >();
	}

	ConsoleHandler handler(stop);
	Package package(&handler);

	try {
		FileIO io(archive);
		package.read(archive, io);
		off_t pos = io.tell();
		io.close();

		if (!filter.empty()) {
			package.filter(filter);
		}

		if (coverage || dump) {
			::coverage(archive, pos, package, dump, directory, humanreadable);
		}
		else if (list) {
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
