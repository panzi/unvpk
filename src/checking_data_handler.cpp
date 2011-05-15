#include <boost/format.hpp>

#include <vpk/exception.h>
#include <vpk/checking_data_handler.h>

void Vpk::CheckingDataHandler::finish() {
	uint32_t sum = m_hash.checksum();
	if (sum != crc32()) {
		throw Exception((boost::format("checksum missmatch, expected 0x%08x, got 0x%08x") % crc32() % sum).str());
	}
}
