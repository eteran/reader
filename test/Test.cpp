
#include "reader/Reader.h"
#include <cassert>
#include <iostream>
#include <regex>

namespace {

struct Token {
	enum Type {
		Identifier,
		Number,
		Delimeter,
	} type;

	std::string value;
};

const std::regex integer_regex(R"((0|[1-9][0-9]*))");
const std::regex identifier_regex(R"([_a-zA-Z$][_a-zA-Z0-9]*)");
const std::regex whitespace_regex(R"([ \t\r\n]+|//.+)");

const char input[] = R"(
int main() {
	// A C++ style comment
	return /* A C style
	multi-line comment */ 42;
}
)";

}

void test_lex_cpp() {
	std::vector<Token> tokens;

	Reader reader(input);

	while (true) {

		// we always consume whitespace (including newlines) between tokens
		while (true) {

			// consume multiline comments while we can
			if (reader.match("/*")) {
				while (!reader.match("*/")) {
					reader.read();
				}
			}

			if (!reader.match(whitespace_regex)) {
				break;
			}
		}

		// if we reach end of the stream, we're done!
		if (reader.eof()) {
			break;
		}

		// extract the next token...
		if (auto ident = reader.match(identifier_regex)) {
			tokens.push_back({Token::Identifier, *ident});
		} else if (auto integer = reader.match(integer_regex)) {
			tokens.push_back({Token::Number, *integer});
		} else if (reader.match('(')) {
			tokens.push_back({Token::Delimeter, "("});
		} else if (reader.match(')')) {
			tokens.push_back({Token::Delimeter, ")"});
		} else if (reader.match('{')) {
			tokens.push_back({Token::Delimeter, "{"});
		} else if (reader.match('}')) {
			tokens.push_back({Token::Delimeter, "}"});
		} else if (reader.match(';')) {
			tokens.push_back({Token::Delimeter, ";"});
		} else {
			std::cout << "Unexpected character: " << reader.peek() << std::endl;
			abort();
		}
	}

	// assert that we got what we expected..
	assert(tokens[0].type == Token::Identifier);
	assert(tokens[0].value == "int");

	assert(tokens[1].type == Token::Identifier);
	assert(tokens[1].value == "main");

	assert(tokens[2].type == Token::Delimeter);
	assert(tokens[2].value == "(");

	assert(tokens[3].type == Token::Delimeter);
	assert(tokens[3].value == ")");

	assert(tokens[4].type == Token::Delimeter);
	assert(tokens[4].value == "{");

	assert(tokens[5].type == Token::Identifier);
	assert(tokens[5].value == "return");

	assert(tokens[6].type == Token::Number);
	assert(tokens[6].value == "42");

	assert(tokens[7].type == Token::Delimeter);
	assert(tokens[7].value == ";");

	assert(tokens[8].type == Token::Delimeter);
	assert(tokens[8].value == "}");
}

void test_match_while() {

	Reader reader("-123_456:789");

	auto key = reader.match_while([](char ch) {
		return std::isalnum(ch) || ch == '-' || ch == '_';
	});

	if (!reader.match(':')) {
		assert(false);
	}

	auto value = reader.match_while([](char ch) {
		return std::isalnum(ch) || ch == '-' || ch == '_';
	});

	assert(key);
	assert(*key == "-123_456");
	assert(value);
	assert(*value == "789");

	std::cout << *key << std::endl;
	std::cout << *value << std::endl;
}

int main() {
	test_lex_cpp();
	test_match_while();
}
