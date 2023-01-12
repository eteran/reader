cmake_minimum_required(VERSION 3.5)

add_library(libReader INTERFACE)

target_sources(libReader
	INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/include/reader/Reader.h
)

target_include_directories(libReader
	INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/include
)
