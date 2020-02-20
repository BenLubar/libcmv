cmake_minimum_required(VERSION 3.0)

project(libcmv VERSION 0.1.0
	DESCRIPTION "Parser for Dwarf Fortress Curses MoVie files"
	HOMEPAGE_URL https://git.lubar.me/ben/libcmv
	LANGUAGES C
)

option(BUILD_SHARED_LIBS "Build libcmv as a shared library." ON)

find_package(ZLIB REQUIRED)

add_library(cmv
	src/context.c
	src/file.c
	src/header.c
	src/sounds.c
	src/sprite.c
	src/sprite_data.c
	src/zlib.c
)

include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(cmv PROPERTIES
	FRAMEWORK ON
	POSITION_INDEPENDENT_CODE ON
	PUBLIC_HEADER include/cmv.h
	VERSION ${PROJECT_VERSION}
	SOVERSION 0
)
target_link_libraries(cmv PUBLIC ZLIB::ZLIB)

install(TARGETS cmv)
