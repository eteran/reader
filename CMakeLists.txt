cmake_minimum_required(VERSION 3.5)
project(Reader VERSION 1.0.0 LANGUAGES CXX)

enable_testing()

add_library(Reader
	Reader.cpp
	Reader.h
)

target_include_directories(Reader
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(Reader
	PROPERTIES
	CXX_STANDARD 17
	CXX_EXTENSIONS OFF
)

add_subdirectory(test)
