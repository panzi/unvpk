#ifndef VPK_CHECKING_DATA_HANDLER_FACTORY_H
#define VPK_CHECKING_DATA_HANDLER_FACTORY_H

#include <vpk/checking_data_handler.h>
#include <vpk/data_handler_factory.h>

namespace Vpk {
	class CheckingDataHandlerFactory : public DataHandlerFactory {
	public:
		CheckingDataHandler *create(const std::string &path, uint32_t crc32) {
			return new CheckingDataHandler(path, crc32);
		}
	};
}

#endif
