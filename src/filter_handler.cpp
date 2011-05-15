#include <boost/filesystem/operations.hpp>

#include <vpk/filter_handler.h>

namespace fs = boost::filesystem;

Vpk::FilterHandler::FilterHandler(const std::vector<std::string> filter) : m_filter(!filter.empty()) {
	for (std::vector<std::string>::const_iterator i = filter.begin(); i != filter.end(); ++ i) {
		if (!i->empty() && (*i)[i->size()-1] == '/') {
			m_dirfilter.insert(*i);
		}
		else {
			m_filefilter.insert(*i);
			m_dirfilter.insert(fs::path(*i).parent_path().string());
			m_dirfilter.insert(*i + "/");
		}
	}
}

Vpk::FilterType Vpk::FilterHandler::filterdir(const std::string &path) {
	if (m_filter) {
		if (m_dirfilter.find(path) != m_dirfilter.end()) {
			return Vpk::ALL;
		}
		else {
			return Vpk::SKIP;
		}
	}
	else {
		return Vpk::ALL;
	}
}

bool Vpk::FilterHandler::filterfile(const std::string &path) {
	return !m_filter || m_filefilter.find(path) != m_filefilter.end();
}
