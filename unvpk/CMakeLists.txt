cmake_minimum_required(VERSION 2.0)

project(unvpk)

include_directories("../libvpk/include" "include")

add_executable(unvpk
	src/main.cpp
	src/archive_stat.cpp
	src/coverage.cpp
	src/console_table.cpp
	src/magic.cpp
	src/multipart_magic.cpp
	src/sorter.cpp
)

install(TARGETS unvpk
	RUNTIME DESTINATION bin
)

target_link_libraries(unvpk
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  libvpk
)
