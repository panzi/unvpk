cmake_minimum_required(VERSION 2.0)

project(vpkfs)

# what does this do? cmake told me to add these three lines:
if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif()

find_package(PkgConfig)
pkg_check_modules(FUSE fuse REQUIRED)
add_definitions(${FUSE_CFLAGS_OTHER})

find_package(Boost COMPONENTS system filesystem program_options REQUIRED)

include_directories("../libvpk/include" "include")

add_executable(vpkfs
	src/main.cpp
	src/vpkfs.cpp
)

install(TARGETS vpkfs
	RUNTIME DESTINATION bin
)

target_link_libraries(vpkfs
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  ${Fuse_LIBRARY}
  libvpk
  fuse
)
