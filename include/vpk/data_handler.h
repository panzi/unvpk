#ifndef VPK_DATA_HANDLER_H
#define VPK_DATA_HANDLER_H

#include <stdint.h>

#include <string>

namespace Vpk {
	class DataHandler {
	public:
		DataHandler(const std::string &path, uint32_t crc32) :
			m_path(path), m_crc32(crc32) {}

		virtual ~DataHandler() {}

		virtual void process(const char *buffer, size_t length) = 0;
		virtual void finish() = 0;
	
		const std::string &path()  const { return m_path; }
		const uint32_t     crc32() const { return m_crc32; }

	private:
		std::string m_path;
		uint32_t    m_crc32;
	};
}

#endif
