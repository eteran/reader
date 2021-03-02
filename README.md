# reader
A class that makes writing a parser programatically easy

## Common usage patterns:

### Tokenization

```
Reader reader(source);

// recommended that the tokens are sorted by length because the first match wins
if (reader.match("++")) {
	// make increment token
} else if (reader.match("--")) {
	// make decrement token
}
...
```

### Reading numbers (using a regex)

```
if (isdigit(reader.peek())) {
	static const std::regex integer_regex(R"(^(0|[1-9][0-9]*))");
	auto number = reader.match(integer_regex);
	if (!number) {
		return invalid_numeric_constant();
	}

	// make sure that this is a valid integer that won't overflow
	// when converted to an integer
	try {
		(void)std::stoi(*number, nullptr, 10);
	} catch (const std::out_of_range &ex) {
		(void)ex;
		return overflow_in_numeric_constant();
	}
	
	// create number token or convert to integer here
}
```

### Match characters based on a predicate


```
// This effectively matches the regex: "[a-zA-Z0-9_-]+"
auto key = reader.match_while([](char ch) {
	return isalnum(ch) || ch == '-' || ch == '_';
});

if (!key) {
    return malformed_key();
}
use_character(*key);
```

### skipping whitespace
```
reader.consume_whitespace(); // skips ' ' and '\t'
 ```
