
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

#include "parser_classes.inc"
#include "parser_error.h"

#include "helper/parameter_pack.h"

#include <concepts>
#include <memory>


namespace clox::parsing
{

struct nil_value_tag_type
{
};

static inline constexpr nil_value_tag_type nil_value_tag{};

class parser final
{
public:
	using token = scanning::token;

	[[nodiscard]] explicit parser(std::vector<scanning::token>&& tokens)
			: tokens_{ tokens }
	{
	}

	std::shared_ptr<expression> parse();

private:
	std::shared_ptr<expression> expr();

	std::shared_ptr<expression> equality();

	std::shared_ptr<expression> comparison();

	std::shared_ptr<expression> term();

	std::shared_ptr<expression> factor();

	std::shared_ptr<expression> unary();

	std::shared_ptr<expression> primary();

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

