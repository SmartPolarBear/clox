
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

#include "parser/parser.h"
#include "scanner/scanner.h"

#include <vector>

using namespace clox::parsing;
using namespace clox::scanning;

using namespace std;

std::shared_ptr<expression> clox::parsing::parser::expr()
{
	return equality();
}

std::shared_ptr<expression> clox::parsing::parser::equality()
{
	auto expr = comparison();
	while (match({ token_type::BANG_EQUAL, token_type::EQUAL_EQUAL }))
	{
		token op = previous();
		auto right = comparison();

		expr = make_shared<binary_expression>(expr, op, right);
	}

	return expr;
}

std::shared_ptr<expression> clox::parsing::parser::comparison()
{
	auto expr = term();

	while (match({ token_type::GREATER, token_type::GREATER_EQUAL, token_type::LESS, token_type::LESS_EQUAL }))
	{
		token op = previous();
		auto right = term();

		expr = make_shared<binary_expression>(expr, op, right);
	}

	return expr;
}

std::shared_ptr<expression> parser::term()
{
	auto expr = factor();

	while (match({ token_type::MINUS, token_type::PLUS }))
	{
		token op = previous();
		auto right = factor();
		expr = make_shared<binary_expression>(expr, op, right);
	}

	return expr;
}

std::shared_ptr<expression> parser::factor()
{
	auto expr = unary();

	while (match({ token_type::SLASH, token_type::STAR }))
	{
		auto op = previous();
		auto right = unary();
		expr = make_shared<binary_expression>(expr, op, right);

	}

	return expr;
}

std::shared_ptr<expression> parser::unary()
{
	if (match({ token_type::BANG, token_type::SLASH }))
	{
		auto op = previous();
		auto right = unary();

		return make_shared<unary_expression>(op, right);
	}

	return primary();
}

std::shared_ptr<expression> parser::primary()
{
	if (match({ token_type::FALSE }))return make_shared<literal>(false);
	if (match({ token_type::TRUE }))return make_shared<literal>(true);
	if (match({ token_type::NIL }))return make_shared<literal>(nullptr);

	if (match({ token_type::NUMBER, token_type::STRING }))
	{
		return make_shared<literal>(previous().literal());
	}

	if (match({ token_type::LEFT_PAREN }))
	{
		auto expr = make_shared<expression>();
		// TODO:

		return make_shared<grouping>(expr);
	}

	return nullptr;
}
