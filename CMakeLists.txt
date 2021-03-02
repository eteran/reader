cmake_minimum_required(VERSION 3.5)
project(Reader VERSION 1.0.0 LANGUAGES CXX)

add_library(Reader
	Reader.cpp
	Reader.h
)

set_target_properties(Reader 
PROPERTIES 
	CXX_STANDARD 14
	CXX_EXTENSIONS OFF
)



