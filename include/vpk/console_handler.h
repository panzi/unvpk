#ifndef VPK_CONSOLE_HANDLER_H
#define VPK_CONSOLE_HANDLER_H

#include <boost/format.hpp>

#include <vpk/package.h>
#include <vpk/filter_handler.h>

namespace Vpk {
	class ConsoleHandler : public FilterHandler {
	public:
		typedef FilterHandler super_type;

		ConsoleHandler(const std::vector<std::string> filter, bool raise = false) :
			FilterHandler(filter), m_raise(raise), m_filecount(0), m_success(0), m_fail(0) {}

		void begin(const Package &package) { m_filecount = package.filecount(); }
		void end();
		
		void direrror(const std::exception &exc);
		void fileerror(const std::exception &exc);
		bool filterfile(const std::string &path);
		void success(const std::string &filepath) { ++ m_success; println(); }
	
		bool         raise()     const { return m_raise; }
		unsigned int success()   const { return m_success; }
		unsigned int fail()      const { return m_fail; }
		unsigned int processed() const { return m_success + m_fail; }
		double       progress()  const { return (double) processed() / (double) m_filecount; }

		void print(const std::string &msg);
		void print(const boost::format &msg) { print(msg.str()); }
		void println() { std::cout << std::endl; }
		void println(const std::string &msg) { print(msg); println(); }
		void println(const boost::format &msg) { println(msg.str()); }

	private:
		bool         m_raise;
		unsigned int m_filecount;
		unsigned int m_success;
		unsigned int m_fail;
	};
}

#endif
