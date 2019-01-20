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

#include <vpk/console_table.h>

void Vpk::ConsoleTable::addColumn(Alignment col) {
	m_alignments.push_back(col);
	if (m_alignments.size() > m_columns) {
		m_columns = m_alignments.size();
	}
}

void Vpk::ConsoleTable::columns(Alignment col1) {
	m_alignments.clear();
	addColumn(col1);
}

void Vpk::ConsoleTable::columns(
		Alignment col1,
		Alignment col2) {
	m_alignments.clear();
	addColumn(col1);
	addColumn(col2);
}

void Vpk::ConsoleTable::columns(
		Alignment col1,
		Alignment col2,
		Alignment col3) {
	m_alignments.clear();
	addColumn(col1);
	addColumn(col2);
	addColumn(col3);
}

void Vpk::ConsoleTable::columns(
		Alignment col1,
		Alignment col2,
		Alignment col3,
		Alignment col4) {
	m_alignments.clear();
	addColumn(col1);
	addColumn(col2);
	addColumn(col3);
	addColumn(col4);
}

void Vpk::ConsoleTable::columns(
		Alignment col1,
		Alignment col2,
		Alignment col3,
		Alignment col4,
		Alignment col5) {
	m_alignments.clear();
	addColumn(col1);
	addColumn(col2);
	addColumn(col3);
	addColumn(col4);
	addColumn(col5);
}

void Vpk::ConsoleTable::columns(
		Alignment col1,
		Alignment col2,
		Alignment col3,
		Alignment col4,
		Alignment col5,
		Alignment col6) {
	m_alignments.clear();
	addColumn(col1);
	addColumn(col2);
	addColumn(col3);
	addColumn(col4);
	addColumn(col5);
	addColumn(col6);
}

void Vpk::ConsoleTable::columns(
		Alignment col1,
		Alignment col2,
		Alignment col3,
		Alignment col4,
		Alignment col5,
		Alignment col6,
		Alignment col7) {
	m_alignments.clear();
	addColumn(col1);
	addColumn(col2);
	addColumn(col3);
	addColumn(col4);
	addColumn(col5);
	addColumn(col6);
	addColumn(col7);
}

void Vpk::ConsoleTable::columns(
		Alignment col1,
		Alignment col2,
		Alignment col3,
		Alignment col4,
		Alignment col5,
		Alignment col6,
		Alignment col7,
		Alignment col8) {
	m_alignments.clear();
	addColumn(col1);
	addColumn(col2);
	addColumn(col3);
	addColumn(col4);
	addColumn(col5);
	addColumn(col6);
	addColumn(col7);
	addColumn(col8);
}

void Vpk::ConsoleTable::columns(const Alignments &alignments) {
	m_alignments = alignments;
	if (m_alignments.size() > m_columns) {
		m_columns = m_alignments.size();
	}
}

void Vpk::ConsoleTable::row(const Row &row) {
	m_body.push_back(row);
	if (row.size() > m_columns) {
		m_columns = row.size();
	}
}

void Vpk::ConsoleTable::fillAlignments(Alignment alignment) {
	fillAlignments(m_columns, alignment);
}

void Vpk::ConsoleTable::fillAlignments(size_t columns, Alignment alignment) {
	for (size_t cols = m_alignments.size(); cols < columns; ++ cols) {
		m_alignments.push_back(alignment);
	}
	if (columns > m_columns) {
		m_columns = columns;
	}
}

void Vpk::ConsoleTable::print(std::ostream &os) const {
	Alignments alignments(m_alignments);
	std::vector<size_t> colsizes(m_columns, 0);
	for (size_t cols = alignments.size(); cols < m_columns; ++ cols) {
		alignments.push_back(LEFT);
	}

	for (Table::const_iterator i = m_body.begin(); i != m_body.end(); ++ i) {
		const Row &row = *i;
		for (size_t j = 0, n = row.size(); j < n; ++ j) {
			size_t size = row[j].size();
			if (size > colsizes[j]) {
				colsizes[j] = size;
			}
		}
	}
	
	for (Table::const_iterator i = m_body.begin(); i != m_body.end(); ++ i) {
		const Row &row = *i;
		for (size_t j = 0, n = row.size(); j < n; ++ j) {
			const std::string &cell = row[j];
			Alignment alignment = alignments[j];

			if (j + 1 < n) {
				align(os, cell, colsizes[j], alignment);
				os << m_delim;
			}
			else {
				align(os, cell, alignment == LEFT ? cell.size() : colsizes[j], alignment);
			}
		}
		os.put('\n');
	}
}

void Vpk::ConsoleTable::align(
		std::ostream &os,
		const std::string &s,
		size_t len,
		Alignment alignment,
		bool truncate,
		char fill) {
	size_t size = s.size();
	if (size < len) {
		switch (alignment) {
		case RIGHT:
			while (size < len) { os.put(fill); ++ size; }
			os << s;
			break;

		case CENTER:
			{
				size_t n = (len - size) / 2;
				size += n;
				while (n) { os.put(fill); -- n; }
			}
			[[fallthrough]];

		case LEFT:
		default:
			os << s;
			while (size < len) { os.put(fill); ++ size; }
			break;
		}
	}
	else if (size == len || !truncate) {
		os << s;
	}
	else if (len <= 3) {
		os.write("...", len);
	}
	else if (alignment == RIGHT) {
		os << "...";
		os.write(s.c_str() + len - size, len);
	}
	else {
		os.write(s.c_str(), len - 3);
		os << "...";
	}
}

void Vpk::ConsoleTable::clear() {
	m_columns = 0;
	m_alignments.clear();
	m_body.clear();
}
