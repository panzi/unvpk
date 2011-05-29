cmake_minimum_required(VERSION 2.0)

project(vpk)

option(WITH_UNVPK "Build unvpk" ON)

find_package(PkgConfig)

if(PKG_CONFIG_FOUND)
	pkg_check_modules(FUSE fuse)
	if(FUSE_FOUND)
		option(WITH_VPKFS "Build vpkfs" ON)
	else()
		option(WITH_VPKFS "Build vpkfs" OFF)
	endif()
else()
	option(WITH_VPKFS "Build vpkfs" OFF)
endif()

if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-long-long -O2")
endif()

# Offsets in VPK archives are uint32_t but off_t might be int32_t on 32bit
# platforms. This should prevent the case where an offset really needs all
# 32bits (when an entry starts at a 2GB offset).
add_definitions(-D_FILE_OFFSET_BITS=64)

add_subdirectory(libvpk)

if(WITH_UNVPK)
	find_package(Boost COMPONENTS system filesystem program_options REQUIRED)
	add_subdirectory(unvpk)
else()
	find_package(Boost COMPONENTS system filesystem REQUIRED)
endif()

if(WITH_VPKFS)
	add_subdirectory(vpkfs)
endif()

# uninstall target
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	IMMEDIATE @ONLY)

add_custom_target(uninstall
	COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
