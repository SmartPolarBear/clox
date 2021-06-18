
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

#include <parser/gen/parser_classes.inc>
#include <parser/parser_error.h>
#include <scanner/nil_value.h>

#include <helper/parameter_pack.h>

#include <concepts>
#include <memory>
#include <vector>


namespace clox::parsing
{


class parser final
{
public:
	using token = scanning::token;

	[[nodiscard]] explicit parser(std::vector<scanning::token>&& tokens)
			: tokens_{ tokens }
	{
	}

	std::vector<std::shared_ptr<statement>> parse();

private:
	std::shared_ptr<statement> stmt();

	std::shared_ptr<statement> print_stmt();

	std::shared_ptr<statement> expr_stmt();

	/// expr -> equality
	/// \return
	std::shared_ptr<expression> expr();

	/// equality -> comparison ( ( "!=" | "==" ) comparison )*
	/// \return
	std::shared_ptr<expression> equality();

	/// comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )*
	/// \return
	std::shared_ptr<expression> comparison();

	/// term -> factor ( ( "-" | "+" ) factor )*
	/// \return
	std::shared_ptr<expression> term();

	/// factor -> unary ( ( "/" | "*" ) unary )*
	/// \return
	std::shared_ptr<expression> factor();

	/// unary -> ( "!" | "-" ) unary|primary ;
	/// \return
	std::shared_ptr<expression> unary();

	/// primary -> NUMBER|STRING|"true"|"false"|"nil"|"(" expr ")"
	/// \return
	std::shared_ptr<expression> primary();

	std::shared_ptr<statement> declaration();

	std::shared_ptr<statement> var_declaration();

	void synchronize();

	token consume(scanning::token_type t, std::string msg);

	parse_error error(token t, std::string msg);

	bool match(std::initializer_list<scanning::token_type> types)
	{
		for (const auto& t:types)
		{
			if (check(t))
			{
				[[maybe_unused]]auto _ = advance();
				return true;
			}
		}

		return false;
	}

	[[nodiscard]] bool check(scanning::token_type t)
	{
		if (is_end())return false;
		return peek().type() == t;
	}

	[[nodiscard]]token advance()
	{
		if (!is_end())cur_++;
		return previous();
	}

	[[nodiscard]]bool is_end()
	{
		return peek().type() == scanning::token_type::FEND;
	}

	[[nodiscard]]token peek()
	{
		return tokens_.at(cur_);
	}

	[[nodiscard]]token previous()
	{
		return tokens_.at(cur_ - 1);
	}

	std::vector<token> tokens_;

	size_t cur_{ 0 };
};

}

