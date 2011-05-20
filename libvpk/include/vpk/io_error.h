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
#ifndef VPK_IO_ERROR_H
#define VPK_IO_ERROR_H

#include <string.h>

#include <vpk/exception.h>

namespace Vpk {
	class IOError : public Exception {
	public:
		IOError(int errnum) : Exception(errnum == EOF ?
			"Unexpected end of file" : strerror(errnum)), m_errnum(errnum) {}
		IOError(const char *msg, int errnum) : Exception(msg), m_errnum(errnum) {}
		IOError(const std::string &msg, int errnum) : Exception(msg), m_errnum(errnum) {}
	
		int errnum() const { return m_errnum; }

	private:
		int m_errnum;
	};
}

#endif
