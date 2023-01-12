cmake_minimum_required(VERSION 3.5)
project(Reader VERSION 1.0.0 LANGUAGES CXX)

enable_testing()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(lib)
add_subdirectory(test)
