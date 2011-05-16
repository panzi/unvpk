cmake_minimum_required(VERSION 2.0)

project(unvpk)

find_package(Boost COMPONENTS system filesystem REQUIRED)

include_directories("include")

add_executable(unvpk
	src/main.cpp
	src/dir.cpp
	src/file.cpp
	src/io.cpp
	src/package.cpp
	src/console_handler.cpp
	src/checking_data_handler.cpp
	src/file_data_handler.cpp
)

install(TARGETS unvpk
	RUNTIME DESTINATION bin
)

target_link_libraries(unvpk
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
)
