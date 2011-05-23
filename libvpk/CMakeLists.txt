cmake_minimum_required(VERSION 2.0)

project(libvpk)

include_directories("include")

add_library(libvpk
	src/version.cpp
	src/util.cpp
	src/file_io.cpp
	src/dir.cpp
	src/file.cpp
	src/package.cpp
	src/console_handler.cpp
	src/checking_data_handler.cpp
	src/file_data_handler.cpp
)

target_link_libraries(libvpk
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
)
