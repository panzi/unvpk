cmake_minimum_required(VERSION 2.0)

project(unvpk)

include_directories("../libvpk/include" "include")

add_executable(unvpk
	src/main.cpp
	src/console_table.cpp
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
