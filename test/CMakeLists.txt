cmake_minimum_required(VERSION 3.0)

add_executable(Reader-test
	Test.cpp
)

target_link_libraries(Reader-test
	Reader
)

set_target_properties(Reader-test
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
	CXX_STANDARD 17
	CXX_EXTENSIONS OFF
)

add_test(
	NAME Reader-test
	COMMAND $<TARGET_FILE:Reader-test>
)
