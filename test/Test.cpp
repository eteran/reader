
#include "reader/Reader.h"
#include <cassert>
#include <iostream>
#include <regex>

namespace {

struct Token {
	enum Type {
		Delimeter,
		Identifier,
		Keyword,
		Number,
	} type;

	std::string value;
};

const std::regex integer_regex(R"((0|[1-9][0-9]*))");
const std::regex identifier_regex(R"([a-zA-Z_][a-zA-Z0-9_]*)");
const std::regex whitespace_regex(R"([ \t\r\n]+|//.+)");
const std::regex keyword_regex(R"(xor_eq|xor|while|wchar_t|volatile|void|virtual|using|unsigned|union|typename|typeid|typedef|try|true|throw|thread_local|this|template|switch|struct|static_cast|static_assert|static|sizeof|signed|short|return|requires|reinterpret_cast|register|public|protected|private|or_eq|or|operator|nullptr|not_eq|not|noexcept|new|namespace|mutable|long|int|inline|if|goto|friend|for|float|false|extern|export|explicit|enum|else|dynamic_cast|double|do|delete|default|decltype|continue|constinit|constexpr|consteval|const_cast|const|concept|compl|co_yield|co_return|co_await|class|char8_t|char32_t|char16_t|char|catch|case|break|bool|bitor|bitand|auto|asm|and_eq|and|alignof|alignas)");

const char input[] = R"(
/**
 * @brief Entry point to the program
 *
 * @return int
 */
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
		if (auto keyword = reader.match(keyword_regex)) {
			tokens.push_back({Token::Keyword, *keyword});
		} else if (auto ident = reader.match(identifier_regex)) {
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
		} else if (reader.match(':')) {
			tokens.push_back({Token::Delimeter, ":"});
		} else {
			std::cout << "Unexpected character: " << reader.peek() << std::endl;
			abort();
		}
	}

	printf("----------\n");
	for (const Token &token : tokens) {
		printf("%s\n", token.value.c_str());
	}
	printf("----------\n");

	// assert that we got what we expected..
	assert(tokens[0].type == Token::Keyword);
	assert(tokens[0].value == "int");

	assert(tokens[1].type == Token::Identifier);
	assert(tokens[1].value == "main");

	assert(tokens[2].type == Token::Delimeter);
	assert(tokens[2].value == "(");

	assert(tokens[3].type == Token::Delimeter);
	assert(tokens[3].value == ")");

	assert(tokens[4].type == Token::Delimeter);
	assert(tokens[4].value == "{");

	assert(tokens[5].type == Token::Keyword);
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
