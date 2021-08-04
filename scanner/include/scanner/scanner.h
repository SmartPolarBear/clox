// Copyright (c) 2021 SmartPolarBear
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <string>
#include <any>
#include <utility>
#include <vector>
#include <unordered_map>

#include <helper/printable.h>


#include <variant>
#include "nil_value.h"

namespace clox::scanning
{


enum class token_type
{
	// Single-character tokens.
	LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
	COMMA, DOT,
	SEMICOLON, COLON /* : */, SLASH,
	QMARK /* ? */,

	// One or two character tokens.
	BANG, BANG_EQUAL,
	EQUAL, EQUAL_EQUAL,
	GREATER, GREATER_EQUAL,
	LESS, LESS_EQUAL,
	MINUS, MINUS_MINUS,
	PLUS, PLUS_PLUS,
	STAR, STAR_STAR,

	// Two character tokens
	ARROW, /* -> */

	// Literals.
	IDENTIFIER, STRING, NUMBER,

	// Keywords.
	AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
	PRINT, RETURN, BASE, THIS, TRUE, VAR, WHILE, USING,
	SIZEOF, ARRAY,

	FEND // not EOF because it conflicts with Microsoft's STL
};

struct empty_literal_tag
{
};

using literal_value_type = std::variant<long long, long double, bool, std::string, scanning::nil_value_tag_type, empty_literal_tag>;


class validator final
{
public:
	static bool valid_number_literal_component(char c);

	static bool valid_identifier_component(char c);

private:
	static bool isdigit(char c);

	static bool isalpha(char c);
};


class token final
{
public:
	static inline constexpr empty_literal_tag empty_literal;

	[[nodiscard]] token(token_type t, std::string lexeme, literal_value_type lit, size_t line)
			: type_(t), lexeme_(std::move(lexeme)), literal_(std::move(lit)), line_(line)
	{
	}


	~token() = default;

	token(const token&) = default;

	token(token&&) = default;

	token& operator=(const token&) = default;

	[[nodiscard]]token_type type() const
	{
		return type_;
	}

	[[nodiscard]]std::string lexeme() const
	{
		return lexeme_;
	}

	[[nodiscard]] literal_value_type literal() const
	{
		return literal_;
	}

	[[nodiscard]] size_t line() const
	{
		return line_;
	}

private:
	token_type type_;
	std::string lexeme_;
	literal_value_type literal_;
	size_t line_;
};

class scanner final
{
public:
	[[nodiscard]] explicit scanner(std::string src)
			: src_(std::move(src))
	{

	}

	[[nodiscard]]std::vector<token> scan();

	[[nodiscard]]bool is_end() const
	{
		return cur_ >= src_.size();
	}

private:
	std::unordered_map<std::string, token_type> keywords{
			{ "and",    token_type::AND },
			{ "class",  token_type::CLASS },
			{ "else",   token_type::ELSE },
			{ "false",  token_type::FALSE },
			{ "for",    token_type::FOR },
			{ "fun",    token_type::FUN },
			{ "if",     token_type::IF },
			{ "nil",    token_type::NIL },
			{ "or",     token_type::OR },
			{ "print",  token_type::PRINT },
			{ "return", token_type::RETURN },
			{ "base",   token_type::BASE },
			{ "this",   token_type::THIS },
			{ "true",   token_type::TRUE },
			{ "var",    token_type::VAR },
			{ "while",  token_type::WHILE },
			{ "using",  token_type::USING },
			{ "sizeof", token_type::SIZEOF },
			{ "array",  token_type::ARRAY }
	};

	void scan_next_token();

	void add_token(token_type t);

	void add_token(token_type t, const literal_value_type& literal);

	/*can be discarded*/ char advance();

	/// peek the next nth character, starting from 0
	/// \param n
	/// \return
	[[nodiscard]]char peek(size_t n);

	[[nodiscard]]char peek();

	[[nodiscard]]bool match(char expect);

	[[nodiscard]] std::string whole_lexeme();

	void scan_string();

	void scan_number_literal();

	void scan_identifier();

	/// handle block comment like /**/
	void consume_block_comment();

	/// handle normal line comment
	void consume_line_comment();

	std::string src_;
	std::vector<token> tokens_{};
	size_t start_{ 0 }, cur_{ 0 }, line_{ 1 };
};
}