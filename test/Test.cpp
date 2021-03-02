
#include "Reader.h"
#include <cassert>
#include <iostream>
#include <regex>

namespace {

const std::regex integer_regex(R"(^(0|[1-9][0-9]*))");
const std::regex identifier_regex(R"(^[_a-zA-Z$][_a-zA-Z0-9]*)");

const char input[] = R"(
int main() {
	return 42;
}
)";
}

int main() {
	Reader reader(input);

	reader.consume(" \t\n");
	auto type = reader.match(identifier_regex);
	assert(type);

	reader.consume(" \t\n");
	auto name = reader.match(identifier_regex);
	assert(name);

	reader.consume(" \t\n");
	assert(reader.match("("));
	
	reader.consume(" \t\n");
	assert(reader.match(")"));
	
	reader.consume(" \t\n");
	assert(reader.match("{"));
	
	reader.consume(" \t\n");
	auto keyword = reader.match(identifier_regex);
	assert(keyword);
	
	reader.consume(" \t\n");
	auto integer = reader.match(integer_regex);
	assert(integer);
	
	reader.consume(" \t\n");
	assert(reader.match(";"));
	
	reader.consume(" \t\n");
	assert(reader.match("}"));

	assert(*type == "int");
	assert(*name == "main");
	assert(*keyword == "return");
	assert(*integer == "42");

	std::cout << "SUCCESS\n";
}
