
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

#include <parser/parser.h>
#include <scanner/scanner.h>

#include <logger/logger.h>

#include <vector>

using namespace clox::parsing;
using namespace clox::scanning;
using namespace clox::logging;

using namespace std;

std::shared_ptr<expression> clox::parsing::parser::expr()
{
	return assigment();
}

std::shared_ptr<expression> parser::assigment()
{
	auto expr = equality();

	if (match({ token_type::EQUAL }))
	{
		auto equals = previous();
		auto val = assigment();

		if (expr->get_type() == PC_TYPE_var_expression)
		{
			auto name = dynamic_cast<var_expression*>(expr.get())->get_name();
			return make_shared<assignment_expression>(name, val);
		}

		// No throw, because of no need for synchronization
		error(equals, "Invalid assignment to the token.");
	}
	else if (match({ token_type::QMARK }))
	{
		auto qmark = previous();
		auto true_expr = this->expr();
		decltype(true_expr) false_expr{ nullptr };
		if (match({ token_type::COLON }))
		{
			false_expr = this->expr();
			return make_shared<ternary_expression>(expr, true_expr, false_expr);
		}
		else
		{
			error(qmark, "Invalid ternary expression: missing ':' clause.");
		}
	}

	return expr;
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
	if (match({ token_type::FALSE }))return make_shared<literal_expression>(false);
	if (match({ token_type::TRUE }))return make_shared<literal_expression>(true);
	if (match({ token_type::NIL }))return make_shared<literal_expression>(nil_value_tag);

	if (match({ token_type::NUMBER, token_type::STRING }))
	{
		return make_shared<literal_expression>(previous().literal());
	}

	if (match({ token_type::IDENTIFIER }))
	{
		return make_shared<var_expression>(previous());
	}

	if (match({ token_type::LEFT_PAREN }))
	{
		auto expr = make_shared<expression>();
		consume(scanning::token_type::RIGHT_PAREN, "')' is expected after the expression");

		return make_shared<grouping_expression>(expr);
	}

	throw error(peek(), "Expression is expected.");
}

token parser::consume(token_type t, const std::string& msg)
{
	if (check(t))return advance();
	throw error(peek(), msg);
}

parse_error parser::error(token t, const std::string& msg)
{
	logging::logger::instance().error(t, msg);
	return parse_error(msg);
}

void parser::synchronize()
{
	auto _ = advance(); // discard it
	while (!is_end())
	{
		if (previous().type() == token_type::SEMICOLON)return;

		switch (peek().type())
		{
		case token_type::CLASS:
		case token_type::FUN:
		case token_type::VAR:
		case token_type::FOR:
		case token_type::IF:
		case token_type::WHILE:
		case token_type::PRINT:
		case token_type::RETURN:
			return;
		default:
			_ = advance();  // discard it
			continue;
		}
	}
}

std::vector<std::shared_ptr<statement>> parser::parse()
{
	vector<shared_ptr<statement>> stmts{};
	while (!is_end())
	{
		stmts.push_back(declaration());
	}

	return stmts;

}

std::shared_ptr<statement> parser::stmt()
{
	if (match({ token_type::IF }))
	{
		return if_stmt();
	}
	else if (match({ token_type::PRINT }))
	{
		return print_stmt();
	}
	else if (match({ token_type::LEFT_BRACE }))
	{
		return make_shared<block_statement>(block_statement{ block() });
	}
	else
	{
		return expr_stmt();
	}
}

std::shared_ptr<statement> parser::print_stmt()
{
	auto val = expr();
	consume(token_type::SEMICOLON, "';' is expected after a value.");
	return make_shared<print_statement>(val);
}

std::shared_ptr<statement> parser::expr_stmt()
{
	auto e = expr();
	consume(token_type::SEMICOLON, "';' is expected after a value.");
	return make_shared<expression_statement>(e);
}

std::shared_ptr<statement> parser::declaration()
{
	try
	{
		if (match({ token_type::VAR }))
		{
			return var_declaration();
		}

		return stmt();
	}
	catch (const parse_error& pe)
	{
		synchronize();
		return nullptr;
	}
}

std::shared_ptr<statement> parser::var_declaration()
{
	auto name = consume(token_type::IDENTIFIER, "Variable name is expected.");

	decltype(expr()) initializer = nullptr;
	if (match({ token_type::EQUAL }))
	{
		initializer = expr();
	}

	consume(token_type::SEMICOLON, "After variable declaration, ';' is expected.");
	return make_shared<variable_statement>(name, initializer);
}

std::vector<std::shared_ptr<statement>> parser::block()
{
	std::vector<std::shared_ptr<statement>> stmts{};

	while (!check(scanning::token_type::RIGHT_BRACE) && !is_end())
	{
		stmts.push_back(declaration());
	}

	consume(scanning::token_type::RIGHT_BRACE, "'}' is expected after a block.");

	return stmts;
}

std::shared_ptr<statement> parser::if_stmt()
{
	consume(scanning::token_type::LEFT_PAREN, "'(' is expected after 'if'.");
	auto cond = expr();
	consume(scanning::token_type::RIGHT_PAREN, "')' is expected after 'if''s condition.");

	auto true_stmt = stmt();
	decltype(true_stmt) false_stmt{ nullptr };

	if (match({ token_type::ELSE }))
	{
		false_stmt = stmt();
	}

	return make_shared<if_statement>(cond, true_stmt, false_stmt);
}

