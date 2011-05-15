#ifndef VPK_DATA_HANDLER_FACTORY_H
#define VPK_DATA_HANDLER_FACTORY_H

#include <stdint.h>

#include <string>

#include <vpk/data_handler.h>

namespace Vpk {
	class DataHandler;

	class DataHandlerFactory {
	public:
		virtual ~DataHandlerFactory() {}

		virtual DataHandler *create(const boost::filesystem::path &path, uint32_t crc32) {
			return create(path.string(), crc32);
		}

		virtual DataHandler *create(const std::string &path, uint32_t crc32) = 0;
	};
}

#endif
