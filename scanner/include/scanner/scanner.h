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

#include <format>
#include <variant>
#include "nil_value.h"

namespace clox::scanning
{


enum class token_type
{
	// Single-character tokens.
	LEFT_PAREN, RIGHT_PAREN, LEFT_BRACKET /* [ */, RIGHT_BRACKET, LEFT_BRACE /* { */, RIGHT_BRACE,
	COMMA, DOT,
	SEMICOLON, COLON /* : */, SLASH,
	QMARK /* ? */,
	PIPE /* | */,

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
	IDENTIFIER, STRING,
	INTEGER, FLOATING,

	// Keywords.
	AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR, IN,
	PRINT, RETURN, BASE, THIS, TRUE, VAR, CONST, WHILE, USING,
	SIZEOF, LIST, MAP, OPERATOR, CONSTRUCTOR, TRY, CATCH, FINALLY, WITH, MATCH,

	FEND, // not EOF because it conflicts with Microsoft's STL
	VIRTUAL // virtual token which is not generated from code
};

struct empty_literal_tag
{
};

struct virtual_token_tag
{
};

static inline constexpr virtual_token_tag virtual_token;

using integer_literal_type = long long;
using floating_literal_type = long double;
using string_literal_type = std::string;
using boolean_literal_type = bool;

using literal_value_type = std::variant<integer_literal_type,
		floating_literal_type,
		string_literal_type,
		boolean_literal_type,
		scanning::nil_value_tag_type,
		empty_literal_tag>;


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

	[[nodiscard]] explicit token(token_type t, std::string lexeme, literal_value_type lit, size_t line)
			: type_(t), lexeme_(std::move(lexeme)), literal_(std::move(lit)), line_(line)
	{
	}

	[[nodiscard]] explicit token(virtual_token_tag)
			: token(token_type::VIRTUAL, "", empty_literal, 0)
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
	[[nodiscard]] explicit scanner(std::string src);

	[[nodiscard]]std::vector<token> scan();

	[[nodiscard]]bool is_end() const
	{
		return cur_ >= src_.size();
	}

	[[nodiscard]] static std::string keyword_from_type(token_type t);

private:
	static inline std::unordered_map<std::string, token_type> keywords_to_type_{
			{ "and",         token_type::AND },
			{ "class",       token_type::CLASS },
			{ "else",        token_type::ELSE },
			{ "false",       token_type::FALSE },
			{ "for",         token_type::FOR },
			{ "in",          token_type::IN },
			{ "fun",         token_type::FUN },
			{ "if",          token_type::IF },
			{ "nil",         token_type::NIL },
			{ "or",          token_type::OR },
			{ "print",       token_type::PRINT },
			{ "return",      token_type::RETURN },
			{ "base",        token_type::BASE },
			{ "this",        token_type::THIS },
			{ "true",        token_type::TRUE },
			{ "var",         token_type::VAR },
			{ "const",       token_type::CONST },
			{ "while",       token_type::WHILE },
			{ "using",       token_type::USING },
			{ "sizeof",      token_type::SIZEOF },
			{ "list",        token_type::LIST },
			{ "map",         token_type::MAP },
			{ "operator",    token_type::OPERATOR },
			{ "constructor", token_type::CONSTRUCTOR },
			{ "try",         token_type::TRY },
			{ "catch",       token_type::CATCH },
			{ "finally",     token_type::FINALLY },
			{ "with",        token_type::MATCH },

	};

	static inline std::unordered_map<token_type, std::string> type_to_keywords_{};

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

namespace std
{
template<>
struct std::formatter<clox::scanning::token> : std::formatter<std::string>
{
	auto format(const clox::scanning::token& p, format_context& ctx)
	{
		return formatter<string>::format(
				std::format("{}", p.lexeme()), ctx);
	}
};
}