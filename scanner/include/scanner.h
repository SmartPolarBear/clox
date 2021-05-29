#pragma once

#include <string>
#include <any>
#include <utility>
#include <vector>
#include <unordered_map>

namespace clox::scanning
{

enum class token_type
{
	// Single-character tokens.
	LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
	COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

	// One or two character tokens.
	BANG, BANG_EQUAL,
	EQUAL, EQUAL_EQUAL,
	GREATER, GREATER_EQUAL,
	LESS, LESS_EQUAL,

	// Literals.
	IDENTIFIER, STRING, NUMBER,

	// Keywords.
	AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
	PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

	FEND // not EOF because it conflicts with STL
};

struct empty_literal_tag
{
};

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

	[[nodiscard]] token(token_type t, std::string lexeme, std::any lit, size_t line)
			: type_(t), lexeme_(std::move(lexeme)), literal_(std::move(lit)), line_(line)
	{
	}


private:
	token_type type_;
	std::string lexeme_;
	std::any literal_;
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
			{ "super",  token_type::SUPER },
			{ "this",   token_type::THIS },
			{ "true",   token_type::TRUE },
			{ "var",    token_type::VAR },
			{ "while",  token_type::WHILE },
	};

	void scan_next_token();

	void add_token(token_type t);

	void add_token(token_type t, const std::any& literal);

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

	std::string src_;
	std::vector<token> tokens_{};
	size_t start_{ 0 }, cur_{ 0 }, line_{ 1 };
};
}