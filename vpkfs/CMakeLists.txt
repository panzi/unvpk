cmake_minimum_required(VERSION 2.0)

project(vpkfs)

# what does this do? cmake told me to add these three lines:
if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif()

find_path(XATTR_INCLUDE_DIR sys/xattr.h)

if(XATTR_INCLUDE_DIR)
	message(STATUS "Found sys/xattr.h")
else()
	message(FATAL_ERROR "Could not find sys/xattr.h")
endif()

find_package(PkgConfig)
pkg_check_modules(FUSE fuse REQUIRED)
add_definitions(${FUSE_CFLAGS_OTHER})

if(FUSE_VERSION VERSION_LESS 2.9)
	add_definitions("-DFUSE_USE_VERSION=26")
else()
	add_definitions("-DFUSE_USE_VERSION=29")
endif()

include_directories("../libvpk/include" "include" "${XATTR_INCLUDE_DIR}")

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
  ${Fuse_LIBRARY}
  libvpk
  fuse
)
