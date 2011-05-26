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
#ifndef VPK_CONSOLE_HANDLER_H
#define VPK_CONSOLE_HANDLER_H

#include <boost/unordered_set.hpp>
#include <boost/format.hpp>

#include <vpk/package.h>
#include <vpk/handler.h>

namespace Vpk {
	class ConsoleHandler : public Handler {
	public:
		typedef Handler super_type;

		ConsoleHandler(bool raise = false) :
			m_begun(false), m_extracting(false), m_raise(raise),
			m_filecount(0), m_success(0), m_fail(0) {}

		void begin(const Package &package);
		void end();
		
		bool direrror(const std::exception &exc, const std::string &path);
		bool fileerror(const std::exception &exc, const std::string &path);
		bool archiveerror(const std::exception &exc, const std::string &path);
		bool filtererror(const std::exception &exc, const std::string &path);
		void extract(const std::string &filepath);
		void success(const std::string &filepath);
	
		void setRaise(bool raise) { m_raise = raise; }

		bool         ok()        const { return m_fail == 0; }
		bool         allok()     const { return m_fail == 0 && m_success == m_filecount; }
		bool         raise()     const { return m_raise; }
		unsigned int success()   const { return m_success; }
		unsigned int fail()      const { return m_fail; }
		unsigned int processed() const { return m_success + m_fail; }
		double       progress()  const { return (double) processed() / (double) m_filecount; }

		void print(const std::string &msg);
		void print(const boost::format &msg) { print(msg.str()); }
		void println(const std::string &msg) { print(msg); std::cout << std::endl; }
		void println(const boost::format &msg) { println(msg.str()); }

	private:
		typedef boost::unordered_set<std::string> Paths;

		bool         m_begun;
		bool         m_extracting;
		bool         m_raise;
		unsigned int m_filecount;
		unsigned int m_success;
		unsigned int m_fail;
		Paths        m_failedArchs;
	};
}

#endif
