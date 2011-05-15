#ifndef VPK_FILE_DATA_HANDLER_H
#define VPK_FILE_DATA_HANDLER_H

#include <boost/crc.hpp>
#include <boost/filesystem/fstream.hpp>

#include <vpk/checking_data_handler.h>

namespace Vpk {
	class FileDataHandler : public CheckingDataHandler {
	public:
		typedef CheckingDataHandler super_type;

		FileDataHandler(const boost::filesystem::path &path, uint32_t crc32, bool check);

		void process(const char *buffer, size_t length) {
			if (m_check) super_type::process(buffer, length);
			m_os.write(buffer, length);
		}

		void finish() {
			m_os.close();
			if (m_check) super_type::finish();
		}

		bool check() const { return m_check; }
	
	private:
		bool                        m_check;
		boost::filesystem::ofstream m_os;
	};
}

#endif
