/**
 * unvpk - list, check and extract vpk archives
 * Copyright (C) 2011-2013  Mathias Panzenböck <grosser.meister.morti@gmx.net>
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
#include <vpk/archive_stat.h>
#include <vpk/coverage.h>
#include <vpk/magic.h>
#include <vpk/list_entry.h>
#include <vpk/sorter.h>

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

static void printListing(
		const Nodes &nodes,
		const std::vector<std::string> &prefix,
		List &lst,
		size_t &files,
		size_t &dirs,
		size_t &sumsize) {
	for (Nodes::const_iterator it = nodes.begin(); it != nodes.end(); ++ it) {
		Node *node = it->second.get();
		std::vector<std::string> path(prefix);
		path.push_back(node->name());
		if (node->type() == Node::DIR) {
			printListing(((const Dir*) node)->nodes(), path, lst, files, dirs, sumsize);
			++ dirs;
		}
		else {
			File *file = (File *) node;
			lst.push_back(ListEntry(algo::join(path, "/"), file));
			sumsize += file->preload.size() + file->size;
			++ files;
		}
	}
}

static size_t bytes(size_t size) {
	return size;
}

static std::string sizeToString(size_t size, bool humanreadable) {
	return humanreadable ?
		Coverage::humanReadableSize(size) :
		boost::lexical_cast<std::string>(size);
}

template<typename SizeFormatter>
static void fillTable(const List &lst, ConsoleTable &table, SizeFormatter szfmt) {
	for (List::const_iterator i = lst.begin(); i != lst.end(); ++ i) {
		i->insert(table, szfmt);
	}
}

static void printListing(const Package &package, bool humanreadable, const SortKeys &sorting) {
	List lst;
	size_t files = 0, dirs = 0, sumsize = 0;

	printListing(package.nodes(), std::vector<std::string>(), lst, files, dirs, sumsize);

	if (!sorting.empty()) {
		Sorter sorter(sorting);
		std::sort(lst.begin(), lst.end(), sorter);
	}

	ConsoleTable table;
	table.columns(ConsoleTable::RIGHT, ConsoleTable::RIGHT, ConsoleTable::RIGHT, ConsoleTable::RIGHT, ConsoleTable::LEFT);
	table.row("Archive", "CRC32", "Offset", "Size", "Filename");
	if (humanreadable) {
		fillTable(lst, table, Coverage::humanReadableSize);
	}
	else {
		fillTable(lst, table, bytes);
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

typedef std::map<int,ArchiveStat> Stats;

static void archive_stat(const Dir &dir, Stats &stats) {
	for (Dir::const_iterator i = dir.begin(); i != dir.end(); ++ i) {
		Node *node = i->second.get();
		if (node->type() == Node::DIR) {
			archive_stat(*(Dir*) node, stats);
		}
		else {
			File *file = (File*) node;
			stats[file->index].add(*file);
		}
	}
}

static void coverage(
		const Package &package,
		bool dump,
		const fs::path &destdir,
		bool humanreadable,
		bool printall) {
	Stats stats;

	stats[0x7fff].coverage().add(0, package.dataoff());

	std::string prefix = tolower(package.name());
	prefix += '_';
	for (fs::directory_iterator i = fs::directory_iterator(package.srcdir()), end; i != end; ++ i) {
		std::string name = tolower(i->path().filename().string());

		if (boost::starts_with(name, prefix) && boost::ends_with(name, ".vpk")) {
			std::string digits(name, prefix.size(), name.size() - prefix.size() - 4);
			if (digits.size() >= 3) {
				try {
					uint16_t index = boost::lexical_cast<uint16_t>(digits);
					stats[index];
				}
				catch (const boost::bad_lexical_cast&) {}
			}
		}
	}

	archive_stat(package, stats);

	if (dump) {
		create_path(destdir);
	}

	ConsoleTable statsTbl;
	statsTbl.columns(ConsoleTable::LEFT, ConsoleTable::RIGHT, ConsoleTable::RIGHT,
	                 ConsoleTable::RIGHT, ConsoleTable::RIGHT, ConsoleTable::RIGHT,
	                 ConsoleTable::LEFT);
	statsTbl.row("File", "Files", "Size", "Covered", "%", "Missing", "Missing Areas");

	size_t files = 0;
	size_t minPreload = -1;
	size_t maxPreload = 0;
	size_t sumPreload = 0;
	size_t minSize = -1;
	size_t maxSize = 0;
	size_t sumSize = 0;
	size_t uncovered = 0;
	size_t total = 0;
	const size_t magicSize = Magic::maxSize();
	std::vector<char> magic(magicSize, 0);
	for (Stats::const_iterator i = stats.begin(); i != stats.end(); ++ i) {
		fs::path path = package.archivePath(i->first);
		std::string archive = path.filename().string();
		size_t size = fs::file_size(path);

		total += size;
		std::string sizeStr = sizeToString(size, humanreadable);

		const ArchiveStat &archStat = i->second;
		const Coverage &covered = archStat.coverage();

		files += archStat.files();
		if (archStat.minPreload() < minPreload) minPreload = archStat.minPreload();
		if (archStat.maxPreload() > maxPreload) maxPreload = archStat.maxPreload();

		if (archStat.minSize() < minSize) minSize = archStat.minSize();
		if (archStat.maxSize() > maxSize) maxSize = archStat.maxSize();
		
		sumPreload += archStat.sumPreload();
		sumSize += archStat.sumSize();
		Coverage missing = covered.invert(size);
		size_t missingSize = missing.coverage();

		if (!printall && missingSize == 0)
			continue;

		uncovered += missingSize;
		std::string missingSizeStr = sizeToString(missingSize, humanreadable);
		
		size_t coveredSize = covered.coverage();
		std::string coveredSizeStr = sizeToString(coveredSize, humanreadable);

		statsTbl.row(archive, archStat.files(), sizeStr, coveredSizeStr,
			boost::format("%.0lf%%") % (covered.coverage() * (double)100 / size),
			missingSizeStr, missing.str(humanreadable));

		if (dump) {
			std::string prefix = (destdir / archive).string();
			FileIO arch(fs::path(package.srcdir()) / archive, "rb");

			const Coverage::Slices &slices = missing.slices();
			for (Coverage::Slices::const_iterator i = slices.begin(); i != slices.end(); ++ i) {
				arch.seek(i->first, FileIO::SET);
				size_t count = std::min(i->second, magicSize);
				arch.read(&magic[0], count);

				std::string type = Magic::extensionOf(&magic[0], count);
				std::string filename = (boost::format("%s_%lu_%lu.%s") % prefix % i->first % i->second % type).str();
				std::string sizeStr = sizeToString(i->second, humanreadable);
				std::cout << "Dumping " << sizeStr << " to \"" << filename << "\"\n";

				FileIO out(filename, "wb");
				out.write(&magic[0], count);
				if (count < i->second) {
					arch.read(out, i->second - count);
				}
			}
			std::cout << std::endl;
		}
	}

	if (files == 0) {
		minPreload = minSize = 0;
	}

	std::string totalStr = sizeToString(total, humanreadable);

	size_t covered = total - uncovered;
	std::string coveredStr = sizeToString(covered, humanreadable);
	std::string uncoveredStr = sizeToString(uncovered, humanreadable);

	std::string minPreloadStr = sizeToString(minPreload, humanreadable);
	std::string maxPreloadStr = sizeToString(maxPreload, humanreadable);
	std::string avgPreloadStr = sizeToString(sumPreload / files, humanreadable);
	
	std::string minSizeStr = sizeToString(minSize, humanreadable);
	std::string maxSizeStr = sizeToString(maxSize, humanreadable);
	std::string avgSizeStr = sizeToString(sumSize / files, humanreadable);

	ConsoleTable totalsTbl;
	totalsTbl.columns(ConsoleTable::LEFT, ConsoleTable::RIGHT, ConsoleTable::RIGHT);
	totalsTbl.row("Total Files:", files);
	totalsTbl.row("Total Size", totalStr);
	totalsTbl.row("Total Covered:", coveredStr, boost::format("%.0lf%%") % (covered * (double)100 / total));
	totalsTbl.row("Total Missing:", uncoveredStr);

	ConsoleTable sizesTbl;
	sizesTbl.columns(ConsoleTable::LEFT, ConsoleTable::RIGHT, ConsoleTable::RIGHT, ConsoleTable::RIGHT);
	sizesTbl.row("", "Min", "Max", "Avg");
	sizesTbl.row("Preload Size", minPreloadStr, maxPreloadStr, avgPreloadStr);
	sizesTbl.row("File Size", minSizeStr, maxSizeStr, avgSizeStr);

	statsTbl.print(std::cout);
	std::cout.put('\n');
	totalsTbl.print(std::cout);
	std::cout.put('\n');
	sizesTbl.print(std::cout);
}

int main(int argc, char *argv[]) {
	po::options_description desc("Options");
	desc.add_options()
		("help,H",           "print help message")
		("version,v",        "print version information")
		("list,l",           "list archive contents")
		("sort,S",           po::value<std::string>(), "sort listing by a comma separated list of keys:\n"
		                     "    a, archive    archive index\n"
		                     "    c, crc32      CRC32 checksum\n"
		                     "    o, offset     offset in archive\n"
		                     "    s, size       file size\n"
		                     "    n, name       file name\n"
							 "prepend - to the key to indicate descending sort order")
		("human-readable,h", "use human readable file sizes in listing")
		("check,c",          "check CRC32 sums")
		("xcheck,x",         "extract and check CRC32 sums")
		("directory,C",      po::value<std::string>(), "extract files into another directory")
		("stop,s",           "stop on error")
		("coverage",         "coverage analysis of archive data (archive debugging)")
		("all,a",            "also show archives 100% coverage")
		("dump-uncovered",   "dump uncovered areas into files (implies --coverage, archive debugging)");

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
	try {
		po::store(po::command_line_parser(argc, argv).options(opts).positional(pos).run(), vm);
		po::notify(vm);
	}
	catch (const std::exception &exc) {
		std::cerr << "*** error: " << exc.what() << std::endl;
		usage(desc);
		return 1;
	}

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
	bool printall      = vm.count("all")            > 0;

	std::string directory = vm.count("directory") > 0 ? vm["directory"].as<std::string>() : std::string(".");
	std::string archive   = vm.count("archive")   > 0 ? vm["archive"].as<std::string>()   : std::string("-");
	std::vector<std::string> filter;
	SortKeys sorting;
	
	if (vm.count("filter") > 0) {
		filter = vm["filter"].as< std::vector<std::string> >();
	}

	if (vm.count("sort") > 0) {
		std::vector<std::string> strsorting;
		boost::split(strsorting, vm["sort"].as<std::string>(), boost::is_any_of(","));

		bool sortByName = false;
		for (std::vector<std::string>::const_iterator i = strsorting.begin(); i != strsorting.end(); ++ i) {
			std::string key = tolower(*i);
			bool asc = true;

			if (key.size() > 0) {
				if (key[0] == '-') {
					key = key.substr(1);
					asc = false;
				}
				else if (key[0] == '+') {
					key = key.substr(1);
				}
			}

			if (key == "a" || key == "archive") {
				sorting.push_back(asc ? SORT_ARCH : SORT_RARCH);
			}
			else if (key == "c" || key == "crc32") {
				sorting.push_back(asc ? SORT_CRC32 : SORT_RCRC32);
			}
			else if (key == "o" || key == "offset") {
				sorting.push_back(asc ? SORT_OFF : SORT_ROFF);
			}
			else if (key == "s" || key == "size") {
				sorting.push_back(asc ? SORT_SIZE : SORT_RSIZE);
			}
			else if (key == "n" || key == "name") {
				sorting.push_back(asc ? SORT_PATH : SORT_RPATH);
				sortByName = true;
			}
			else {
				std::cerr << "*** error: illegal sort key: \"" << *i << "\"\n";
				return 1;
			}
		}
		if (!sortByName) {
			sorting.push_back(SORT_PATH);
		}
	}

	ConsoleHandler handler(stop);
	Package package(&handler);

	try {
		package.read(archive);

		if (!filter.empty()) {
			package.filter(filter);
		}

		if (coverage || dump) {
			::coverage(package, dump, directory, humanreadable, printall);
		}
		else if (list) {
			printListing(package, humanreadable, sorting);
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
