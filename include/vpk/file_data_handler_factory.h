#ifndef VPK_FILE_DATA_HANDLER_FACTORY_H
#define VPK_FILE_DATA_HANDLER_FACTORY_H

#include <vpk/file_data_handler.h>
#include <vpk/data_handler_factory.h>

namespace Vpk {
	class FileDataHandlerFactory : public DataHandlerFactory {
	public:
		FileDataHandlerFactory(const std::string &destdir, bool check)
		: m_destdir(destdir), m_check(check) {}

		FileDataHandler *create(const boost::filesystem::path &path, uint32_t crc32) {
			return new FileDataHandler(m_destdir / path, crc32, m_check);
		}

		FileDataHandler *create(const std::string &path, uint32_t crc32) {
			return new FileDataHandler(m_destdir / path, crc32, m_check);
		}

		const boost::filesystem::path &destdir() const { return m_destdir; }
		bool check() const { return m_check; }
	
	private:
		boost::filesystem::path m_destdir;
		bool                    m_check;
	};
}

#endif
