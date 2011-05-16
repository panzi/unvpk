#ifndef VPK_CHECKING_DATA_HANDLER_H
#define VPK_CHECKING_DATA_HANDLER_H

#include <boost/crc.hpp>

#include <vpk/data_handler.h>

namespace Vpk {
	class CheckingDataHandler : public DataHandler {
	public:
		CheckingDataHandler(const std::string &path, uint32_t crc32) :
			DataHandler(path, crc32) {}

		void process(const char *buffer, size_t length) {
			m_hash.process_bytes(buffer, length);
		}

		void finish();
	
	private:
		boost::crc_32_type m_hash;
	};
}

#endif
