
#ifndef READER_H_
#define READER_H_

#include <cassert>
#include <cstddef>
#include <optional>
#include <regex>
#include <stack>
#include <string>
#include <string_view>

template <class Ch>
class BasicReader {
public:
	struct Location {
		size_t line;
		size_t column;
	};

public:
	/**
	 * @brief Construct a new Basic Reader object for lexing a string
	 *
	 * @param input
	 */
	explicit BasicReader(std::basic_string_view<Ch> input) noexcept
		: input_(input) {
	}

	BasicReader()                                  = default;
	BasicReader(const BasicReader &other)          = default;
	BasicReader &operator=(const BasicReader &rhs) = default;
	~BasicReader()                                 = default;

public:
	/**
	 * @brief Returns true if the reader is at the end of the stream
	 *
	 * @return bool
	 */
	bool eof() const noexcept {
		return index_ == input_.size();
	}

	/**
	 * @brief Returns the next character in the string without advancing the position
	 *
	 * @return Ch
	 */
	Ch peek() const noexcept {
		if (eof()) {
			return '\0';
		}

		return input_[index_];
	}

	/**
	 * @brief Returns the next character in the string and advances the position
	 *
	 * @return Ch
	 */
	Ch read() noexcept {
		if (eof()) {
			return '\0';
		}

		return input_[index_++];
	}

	/**
	 * @brief Consumes while the next character is in the input set <chars>
	 * and returns the number of consumed characters
	 *
	 * @param chars
	 * @return size_t
	 */
	size_t consume(std::basic_string_view<Ch> chars) noexcept {
		//
		return consume_while([chars](Ch ch) {
			return chars.find(ch) != std::basic_string<Ch>::npos;
		});
	}

	/**
	 * @brief Consumes while the next character is whitespace (tab or space)
	 * and returns the number of consumed characters
	 *
	 * @return size_t
	 */
	size_t consume_whitespace() noexcept {
		//
		return consume_while([](Ch ch) {
			return (ch == ' ' || ch == '\t');
		});
	}

	/**
	 * @brief Consumes while a given predicate function returns true
	 * and returns the number of consumed characters
	 *
	 * @param pred
	 * @return size_t
	 */
	template <class Pred>
	size_t consume_while(Pred pred) noexcept {
		size_t count = 0;
		while (!eof()) {
			Ch ch = peek();

			if (!pred(ch)) {
				break;
			}

			read();
			++count;
		}
		return count;
	}

	/**
	 * @brief Returns true and advances the position
	 * if the next character matches <ch>
	 *
	 * @param ch
	 * @return bool
	 */
	bool match(Ch ch) noexcept {
		if (peek() != ch) {
			return false;
		}

		++index_;
		return true;
	}

	/**
	 * @brief Returns true and advances the position
	 * if the next sequences of characters matches <s>
	 *
	 * @param s
	 * @return bool
	 */
	bool match(std::basic_string_view<Ch> s) noexcept {
		if (index_ + s.size() > input_.size()) {
			return false;
		}

		size_t new_index_ = index_ + s.size();

		for (size_t i = 0; i < s.size(); ++i) {
			const Ch ch = input_[index_ + i];
			if (ch != s[i]) {
				return false;
			}
		}

		index_ = new_index_;
		return true;
	}

	/**
	 * @brief Matches until the end of the input and returns the string matched
	 *
	 * @return std::optional<std::basic_string<Ch>>
	 */
	std::optional<std::basic_string<Ch>> match_any() {
		std::basic_string<Ch> m;
		while (!eof()) {
			m.push_back(read());
		}

		if (!m.empty()) {
			return m;
		}

		return {};
	}

	/**
	 * @brief Returns the matching string and advances the position
	 * if the next sequences of characters matches <regex>
	 *
	 * @param s
	 * @return bool
	 */
	std::optional<std::basic_string<Ch>> match(const std::basic_regex<Ch> &regex) {
		std::match_results<const Ch *> matches;

		const Ch *first = &input_[index_];
		const Ch *last  = &input_[input_.size()];

		if (std::regex_search(first, last, matches, regex, std::regex_constants::match_continuous)) {
			std::basic_string<Ch> m = std::basic_string<Ch>(matches[0].first, matches[0].second);
			index_ += m.size();
			return m;
		}

		return {};
	}

	/**
	 * @brief Returns the matching string and advances the position
	 * for each character satisfying the given predicate
	 *
	 * @param pred
	 * @return std::optional<std::basic_string<Ch>>
	 */
	template <class Pred>
	std::optional<std::basic_string<Ch>> match_while(Pred pred) {
		std::basic_string<Ch> m;
		while (!eof()) {
			const Ch ch = peek();
			if (!pred(ch)) {
				break;
			}
			m.push_back(read());
		}

		if (!m.empty()) {
			return m;
		}

		return {};
	}

	/**
	 * @brief Returns the current position in the string
	 *
	 * @return size_t
	 */
	size_t index() const noexcept {
		return index_;
	}

	/**
	 * @brief Returns the current position in the string as a line/column pair
	 *
	 * @return Location
	 */
	Location location() const noexcept {
		return location(index_);
	}

	/**
	 * @brief Returns the position of <index> in the string as line/column pair
	 *
	 * @param index
	 * @return Location
	 */
	Location location(size_t index) const noexcept {
		size_t line = 1;
		size_t col  = 1;

		if (index < input_.size()) {

			for (size_t i = 0; i < index; ++i) {
				if (input_[i] == '\n') {
					++line;
					col = 1;
				} else {
					++col;
				}
			}
		}

		return Location{line, col};
	}

	/**
	 * @brief Stores the current state of the reader onto the stack
	 *
	 */
	void push_state() {
		state_.push(index_);
	}

	/**
	 * @brief Removes the most recently pushed state from the stack
	 *
	 */
	void pop_state() {
		assert(!state_.empty());
		state_.pop();
	}

	/**
	 * @brief Sets the current state to the most recently pushed state from the stack, and then
	 * removes the most recently pushed state from the stack
	 *
	 */
	void restore_state() {
		assert(!state_.empty());
		index_ = state_.top();
		state_.pop();
	}

private:
	std::basic_string_view<Ch> input_;
	size_t index_ = 0;
	std::stack<size_t> state_;
};

using Reader = BasicReader<char>;

#endif
