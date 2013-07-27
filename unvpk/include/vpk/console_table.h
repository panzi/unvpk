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
#ifndef VPK_CONSOLE_TABLE_H
#define VPK_CONSOLE_TABLE_H

#include <vector>
#include <iostream>

#include <boost/lexical_cast.hpp>

namespace Vpk {	
	class ConsoleTable {
	public:
		enum Alignment {
			LEFT,
			CENTER,
			RIGHT
		};

		typedef std::vector<Alignment>   Alignments;
		typedef std::vector<std::string> Row;
		typedef std::vector<Row>         Table;

		ConsoleTable() : m_columns(0), m_delim("  ") {}

		void addColumn(Alignment col);
		void columns(Alignment col1);
		void columns(Alignment col1, Alignment col2);
		void columns(Alignment col1, Alignment col2, Alignment col3);
		void columns(Alignment col1, Alignment col2, Alignment col3, Alignment col4);
		void columns(Alignment col1, Alignment col2, Alignment col3, Alignment col4, Alignment col5);
		void columns(Alignment col1, Alignment col2, Alignment col3, Alignment col4, Alignment col5, Alignment col6);
		void columns(Alignment col1, Alignment col2, Alignment col3, Alignment col4, Alignment col5, Alignment col6, Alignment col7);
		void columns(Alignment col1, Alignment col2, Alignment col3, Alignment col4, Alignment col5, Alignment col6, Alignment col7, Alignment col8);
		void columns(const Alignments &alignments);

		template<typename Type1>
		void row(const Type1 &col1) {
			m_body.push_back(Row());
	
			Row &row = m_body.back();
			row.push_back(boost::lexical_cast<std::string>(col1));
	
			if (m_columns < row.size()) {
				m_columns = row.size();
			}
		}

		template<
			typename Type1,
			typename Type2>
		void row(
			const Type1 &col1,
			const Type2 &col2) {
			m_body.push_back(Row());
	
			Row &row = m_body.back();
			row.push_back(boost::lexical_cast<std::string>(col1));
			row.push_back(boost::lexical_cast<std::string>(col2));
	
			if (m_columns < row.size()) {
				m_columns = row.size();
			}
		}

		template<
			typename Type1,
			typename Type2,
			typename Type3>
		void row(
			const Type1 &col1,
			const Type2 &col2,
			const Type3 &col3) {
			m_body.push_back(Row());
	
			Row &row = m_body.back();
			row.push_back(boost::lexical_cast<std::string>(col1));
			row.push_back(boost::lexical_cast<std::string>(col2));
			row.push_back(boost::lexical_cast<std::string>(col3));
	
			if (m_columns < row.size()) {
				m_columns = row.size();
			}
		}

		template<
			typename Type1,
			typename Type2,
			typename Type3,
			typename Type4>
		void row(
			const Type1 &col1,
			const Type2 &col2,
			const Type3 &col3,
			const Type4 &col4) {
			m_body.push_back(Row());
	
			Row &row = m_body.back();
			row.push_back(boost::lexical_cast<std::string>(col1));
			row.push_back(boost::lexical_cast<std::string>(col2));
			row.push_back(boost::lexical_cast<std::string>(col3));
			row.push_back(boost::lexical_cast<std::string>(col4));
	
			if (m_columns < row.size()) {
				m_columns = row.size();
			}
		}

		template<
			typename Type1,
			typename Type2,
			typename Type3,
			typename Type4,
			typename Type5>
		void row(
			const Type1 &col1,
			const Type2 &col2,
			const Type3 &col3,
			const Type4 &col4,
			const Type5 &col5) {
			m_body.push_back(Row());
	
			Row &row = m_body.back();
			row.push_back(boost::lexical_cast<std::string>(col1));
			row.push_back(boost::lexical_cast<std::string>(col2));
			row.push_back(boost::lexical_cast<std::string>(col3));
			row.push_back(boost::lexical_cast<std::string>(col4));
			row.push_back(boost::lexical_cast<std::string>(col5));
	
			if (m_columns < row.size()) {
				m_columns = row.size();
			}
		}

		template<
			typename Type1,
			typename Type2,
			typename Type3,
			typename Type4,
			typename Type5,
			typename Type6>
		void row(
			const Type1 &col1,
			const Type2 &col2,
			const Type3 &col3,
			const Type4 &col4,
			const Type5 &col5,
			const Type6 &col6) {
			m_body.push_back(Row());
	
			Row &row = m_body.back();
			row.push_back(boost::lexical_cast<std::string>(col1));
			row.push_back(boost::lexical_cast<std::string>(col2));
			row.push_back(boost::lexical_cast<std::string>(col3));
			row.push_back(boost::lexical_cast<std::string>(col4));
			row.push_back(boost::lexical_cast<std::string>(col5));
			row.push_back(boost::lexical_cast<std::string>(col6));
	
			if (m_columns < row.size()) {
				m_columns = row.size();
			}
		}

		template<
			typename Type1,
			typename Type2,
			typename Type3,
			typename Type4,
			typename Type5,
			typename Type6,
			typename Type7>
		void row(
			const Type1 &col1,
			const Type2 &col2,
			const Type3 &col3,
			const Type4 &col4,
			const Type5 &col5,
			const Type6 &col6,
			const Type7 &col7) {
			m_body.push_back(Row());
	
			Row &row = m_body.back();
			row.push_back(boost::lexical_cast<std::string>(col1));
			row.push_back(boost::lexical_cast<std::string>(col2));
			row.push_back(boost::lexical_cast<std::string>(col3));
			row.push_back(boost::lexical_cast<std::string>(col4));
			row.push_back(boost::lexical_cast<std::string>(col5));
			row.push_back(boost::lexical_cast<std::string>(col6));
			row.push_back(boost::lexical_cast<std::string>(col7));
	
			if (m_columns < row.size()) {
				m_columns = row.size();
			}
		}

		template<
			typename Type1,
			typename Type2,
			typename Type3,
			typename Type4,
			typename Type5,
			typename Type6,
			typename Type7,
			typename Type8>
		void row(
			const Type1 &col1,
			const Type2 &col2,
			const Type3 &col3,
			const Type4 &col4,
			const Type5 &col5,
			const Type6 &col6,
			const Type7 &col7,
			const Type8 &col8) {
			m_body.push_back(Row());
	
			Row &row = m_body.back();
			row.push_back(boost::lexical_cast<std::string>(col1));
			row.push_back(boost::lexical_cast<std::string>(col2));
			row.push_back(boost::lexical_cast<std::string>(col3));
			row.push_back(boost::lexical_cast<std::string>(col4));
			row.push_back(boost::lexical_cast<std::string>(col5));
			row.push_back(boost::lexical_cast<std::string>(col6));
			row.push_back(boost::lexical_cast<std::string>(col7));
			row.push_back(boost::lexical_cast<std::string>(col8));
	
			if (m_columns < row.size()) {
				m_columns = row.size();
			}
		}

		void row(const Row &row);
		void fillAlignments(Alignment alignment = LEFT);
		void fillAlignments(size_t columns, Alignment alignment = LEFT);
		void print(std::ostream &os = std::cout) const;
		static void align(
				std::ostream &os,
				const std::string &s,
				size_t len,
				Alignment alignment = LEFT,
				bool truncate = true,
				char fill = ' ');
	
		size_t             columns()    const { return m_columns; }
		const Alignments  &alignments() const { return m_alignments; }
		const Table       &body()       const { return m_body; }
		const std::string &delimeter()  const { return m_delim; }

		void setDelimeter(const std::string &delimeter) { m_delim = delimeter; }
	
		void clear();
	
	private:
		size_t      m_columns;
		Alignments  m_alignments;
		Table       m_body;
		std::string m_delim;
	};
}

#endif
