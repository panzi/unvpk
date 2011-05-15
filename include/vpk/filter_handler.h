#ifndef VPK_FILTER_HANDLER_H
#define VPK_FILTER_HANDLER_H

#include <string>
#include <vector>
#include <set>

#include <vpk/handler.h>

namespace Vpk {
	class FilterHandler : public Handler {
	public:
		FilterHandler(const std::vector<std::string> filter);

		FilterType filterdir( const std::string &path);
		bool       filterfile(const std::string &path);

		bool filter() const { return m_filter; }
		const std::set<std::string> &dirfilter()  const { return m_dirfilter; }
		const std::set<std::string> &filefilter() const { return m_filefilter; }
	
	private:
		bool                  m_filter;
		std::set<std::string> m_dirfilter;
		std::set<std::string> m_filefilter;
	};
}

#endif
