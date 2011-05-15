#include <vpk/file_data_handler.h>

namespace fs = boost::filesystem;

static void create_path(const fs::path &path) {
	fs::path parent = path.parent_path();
	if (!fs::exists(parent)) {
		create_path(parent);
	}
	create_directory(path);
}

Vpk::FileDataHandler::FileDataHandler(
	const fs::path &path, uint32_t crc32, bool check)
: CheckingDataHandler(path.string(), crc32), m_check(check) {
	create_path(path.parent_path());

	m_os.exceptions(std::ios::failbit | std::ios::badbit);
	m_os.open(path, std::ios::out | std::ios::binary);
}
