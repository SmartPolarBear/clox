
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
#include <format>

using namespace clox::parsing;
using namespace clox::scanning;
using namespace clox::logging;

using namespace std;

std::shared_ptr<expression> clox::parsing::parser::expr()
{
	return comma();
}


std::shared_ptr<expression> parser::comma()
{
	auto expr = conditional();
	while (match({ token_type::COMMA }))
	{
		auto op = previous();
		auto right = conditional();

		expr = make_shared<binary_expression>(expr, op, right);
	}

	return expr;
}

std::shared_ptr<expression> parser::conditional()
{
	auto expr = assigment();
	if (match({ token_type::QMARK }))
	{
		auto qmark = previous();
		auto true_expr = this->expr();
		auto colon = consume(scanning::token_type::COLON, "Invalid ternary expression: missing ':' clause.");
		auto false_expr = this->conditional();
		expr = make_shared<ternary_expression>(expr, qmark, true_expr, colon, false_expr);
	}

	return expr;
}


std::shared_ptr<expression> parser::assigment()
{
	auto expr = logical_or();

	if (match({ token_type::EQUAL }))
	{
		auto equals = previous();
		auto val = assigment();

		if (expr->get_type() == PC_TYPE_var_expression)
		{
			auto name = dynamic_pointer_cast<var_expression>(expr)->get_name();
			return make_shared<assignment_expression>(name, val);
		}
		else if (expr->get_type() == PC_TYPE_get_expression)
		{
			auto ge = static_pointer_cast<get_expression>(expr);
			return make_shared<set_expression>(ge->get_object(), ge->get_name(), val);
		}

		// No throw, because of no need for synchronization
		error(equals, "Invalid assignment to the token.");
	}

	return expr;
}

std::shared_ptr<expression> parser::logical_or()
{
	auto expr = logical_and();

	while (match({ token_type::OR }))
	{
		auto op = previous();
		auto rhs = logical_and();
		expr = make_shared<logical_expression>(expr, op, rhs);
	}

	return expr;
}

std::shared_ptr<expression> parser::logical_and()
{
	auto expr = equality();

	while (match({ token_type::AND }))
	{
		auto op = previous();
		auto rhs = equality();
		expr = make_shared<logical_expression>(expr, op, rhs);
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

	return exponent();
}

std::shared_ptr<expression> parser::exponent()
{
	auto expr = prefix();
	if (match({ token_type::STAR_STAR }))
	{
		auto op = previous();
		auto right = unary();
		return make_shared<binary_expression>(expr, op, right);
	}
	return expr;
}

std::shared_ptr<expression> parser::prefix()
{
	if (match({ token_type::PLUS_PLUS, token_type::MINUS_MINUS }))
	{
		auto op = previous();
		auto right = unary();

		return make_shared<unary_expression>(op, right);
	}

	return postfix();
}

std::shared_ptr<expression> parser::postfix()
{
	if (check(token_type::PLUS_PLUS, 1))
	{
		while (check(token_type::PLUS_PLUS, 1))
		{
			auto op = peek(1);
			auto left = primary();
			consume(token_type::PLUS_PLUS, "Invalid postfix expression");

			return make_shared<postfix_expression>(left, op);
		}
	}
	else if (check(token_type::MINUS_MINUS, 1))
	{
		while (check(token_type::MINUS_MINUS, 1))
		{
			auto op = peek(1);
			auto left = primary();
			consume(token_type::MINUS_MINUS, "Invalid postfix expression");

			return make_shared<postfix_expression>(left, op);
		}
	}


	return call();
}

std::shared_ptr<expression> parser::call()
{
	auto expr = primary();
	while (true)
	{
		if (match({ token_type::LEFT_PAREN }))
		{
			expr = call_finish_parse(expr);
		}
		else if (match({ token_type::DOT }))
		{
			auto name = consume(scanning::token_type::IDENTIFIER, "Property name is expected after '.' .");
			expr = make_shared<get_expression>(expr, name);
		}
		else
		{
			break;
		}
	}

	return expr;
}


std::shared_ptr<expression> parser::call_finish_parse(const shared_ptr<expression>& callee)
{
	vector<shared_ptr<expression>> args{};
	if (!check(scanning::token_type::RIGHT_PAREN))
	{
		do
		{
			if (args.size() > FUNC_ARG_LIST_MAX)
			{
				error(peek(), std::format("Too many arguments. Only {} are allowed.", FUNC_ARG_LIST_MAX));
			}
			args.push_back(this->conditional() /* not expr() because I want skip comma rule */);
		} while (match({ token_type::COMMA }));
	}

	token paren = consume(scanning::token_type::RIGHT_PAREN, "')' is expected for argument list.");
	return make_shared<call_expression>(callee, paren, args);
}


std::shared_ptr<expression> parser::primary()
{
	if (match({ token_type::FALSE }))return make_shared<literal_expression>(previous(), false);
	else if (match({ token_type::TRUE }))return make_shared<literal_expression>(previous(), true);
	else if (match({ token_type::NIL }))return make_shared<literal_expression>(previous(), nil_value_tag);
	else if (match({ token_type::INTEGER, token_type::FLOATING, token_type::STRING }))
	{
		return make_shared<literal_expression>(previous(), previous().literal());
	}
	else if (match({ token_type::BASE }))
	{
		auto keyword = previous();
		consume(scanning::token_type::DOT, "'.' is expected after base.");
		auto method = consume(scanning::token_type::IDENTIFIER, "Identifier is expected after base.");
		return make_shared<base_expression>(keyword, method); // FIXME: we should use get expression !
	}
	else if (match({ token_type::THIS }))return make_shared<this_expression>(previous());
	else if (match({ token_type::IDENTIFIER }))
	{
		return make_shared<var_expression>(previous());
	}
	else if (match({ token_type::LEFT_PAREN }))
	{
		auto expr = this->expr();
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
	[[maybe_unused]] auto _discard1 = advance(); // discard it
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
			[[maybe_unused]] auto _discard2 = advance();  // discard it
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
	else if (match({ token_type::FOR }))
	{
		return for_stmt();
	}
	else if (match({ token_type::PRINT }))
	{
		return print_stmt();
	}
	else if (match({ token_type::RETURN }))
	{
		return return_stmt();
	}
	else if (match({ token_type::WHILE }))
	{
		return while_stmt();
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
	auto keyword = previous();
	auto val = expr();
	consume(token_type::SEMICOLON, "';' is expected after a value.");
	return make_shared<print_statement>(keyword, val);
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
		if (match({ token_type::CLASS }))
		{
			return class_declaration();
		}
		else if (match({ token_type::FUN }))
		{
			return func_declaration("function");
		}
		else if (match({ token_type::VAR }))
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

std::shared_ptr<statement> parser::func_declaration(const std::string& kind)
{

	auto name = consume(token_type::IDENTIFIER, std::format("{} name is expected.", kind));
	consume(token_type::LEFT_PAREN, std::format("'(' is expected after {} name.", kind));

	std::vector<std::pair<clox::scanning::token, std::shared_ptr<type_expression>>> params{};

	if (!check(scanning::token_type::RIGHT_PAREN))
	{
		do
		{
			if (params.size() >= FUNC_ARG_LIST_MAX)
			{
				error(peek(), std::format("Too many arguments. Only {} are allowed.", FUNC_ARG_LIST_MAX));
			}

			auto param = consume(scanning::token_type::IDENTIFIER, "Parameter names are expected.");
			consume(token_type::COLON, std::format("Type is required for parameter {}", param.lexeme()));
			auto param_type = type_expr();

			params.emplace_back(param, param_type);
		} while (match({ token_type::COMMA }));
	}

	consume(token_type::RIGHT_PAREN, std::format("')' is expected after {} name.", kind));

	decltype(type_expr()) ret_type{ nullptr };
	if (peek().type() == token_type::COLON)
	{
		consume(token_type::COLON, std::format("Return type is required for callable {}", name.lexeme()));
		ret_type = type_expr();
	}

	consume(token_type::LEFT_BRACE, std::format("'{{' is expected after {} declaration and before its body.", kind));

	auto body = block();

	return make_shared<function_statement>(name, params, ret_type, body);
}

std::shared_ptr<statement> parser::var_declaration()
{
	auto name = consume(token_type::IDENTIFIER, "Variable name is expected.");

	decltype(type_expr()) type = nullptr;
	if (match({ token_type::COLON }))
	{
		type = type_expr();
	}

	decltype(expr()) initializer = nullptr;
	if (match({ token_type::EQUAL }))
	{
		initializer = expr();
	}

	consume(token_type::SEMICOLON, "After variable declaration, ';' is expected.");
	return make_shared<variable_statement>(name, type, initializer);
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
	auto lparen = consume(scanning::token_type::LEFT_PAREN, "'(' is expected after 'if'.");
	auto cond = expr();
	consume(scanning::token_type::RIGHT_PAREN, "')' is expected after 'if''s condition.");

	auto true_stmt = stmt();
	decltype(true_stmt) false_stmt{ nullptr };

	if (match({ token_type::ELSE }))
	{
		false_stmt = stmt();
	}

	return make_shared<if_statement>(cond, lparen, true_stmt, false_stmt);
}

std::shared_ptr<statement> parser::return_stmt()
{
	auto ret_keyword = previous();
	shared_ptr<expression> val{ nullptr };
	if (!check(scanning::token_type::SEMICOLON))
	{
		val = this->expr();
	}

	consume(scanning::token_type::SEMICOLON, "';' is expected after return value.");
	return make_shared<return_statement>(ret_keyword, val);
}


std::shared_ptr<statement> parser::while_stmt()
{
	auto lparen = consume(scanning::token_type::LEFT_PAREN, "'(' is expected after 'while'.");
	auto cond = this->expr();
	consume(scanning::token_type::RIGHT_PAREN, "')' is expected after 'while''s condition.");

	auto body = stmt();

	return make_shared<while_statement>(cond, lparen, body);
}

std::shared_ptr<statement> parser::for_stmt()
{
	auto lparen = consume(scanning::token_type::LEFT_PAREN, "'(' is expected after 'for'.");

	shared_ptr<statement> initializer{ nullptr };
	if (match({ token_type::SEMICOLON }))
	{
		// Do nothing
	}
	else if (match({ token_type::VAR }))
	{
		initializer = var_declaration();
	}
	else
	{
		initializer = expr_stmt();
	}

	shared_ptr<expression> cond{ nullptr };
	if (!check(token_type::SEMICOLON))
	{
		cond = expr();
	}
	consume(scanning::token_type::SEMICOLON, "';' is expected after the condition of for loop.");

	shared_ptr<expression> increment{ nullptr };
	if (!check(scanning::token_type::RIGHT_PAREN))
	{
		increment = expr();
	}
	consume(scanning::token_type::RIGHT_PAREN, "')' is expected after for clauses.");

	auto body = stmt();

	// de-sugaring the for loop to a while loop

	if (increment)
	{
		body = make_shared<block_statement>(vector<shared_ptr<statement>>{
				body,
				make_shared<expression_statement>(increment) });
	}

	if (!cond)cond = make_shared<literal_expression>(lparen, true);
	body = make_shared<while_statement>(cond, lparen, body);

	if (initializer)
	{
		body = make_shared<block_statement>(vector<shared_ptr<statement>>{
				initializer,
				body });
	}

	return body;
}

std::shared_ptr<statement> parser::class_declaration()
{
	auto name = consume(scanning::token_type::IDENTIFIER, "Class name is expected.");

	shared_ptr<var_expression> base_class{ nullptr };
	if (match({ token_type::COLON }))
	{
		consume(scanning::token_type::IDENTIFIER, "Base class name is expected.");
		base_class = make_shared<var_expression>(previous());
	}

	consume(scanning::token_type::LEFT_BRACE, "'{' is expected after class name.");

	vector<shared_ptr<function_statement>> methods{};
	vector<shared_ptr<variable_statement>> fields{};

	while (!check(scanning::token_type::RIGHT_BRACE) && !is_end())
	{
		if (match({ scanning::token_type::VAR }))
		{
			fields.push_back(static_pointer_cast<variable_statement>(var_declaration()));
		}
		else if (match({ scanning::token_type::FUN }))
		{
			methods.push_back(static_pointer_cast<function_statement>(func_declaration("method")));
		}
		else
		{
			throw error(peek(), "Field or method is expected.");
		}
	}

	consume(scanning::token_type::RIGHT_BRACE, "'}' is expected after class body.");
	return make_shared<class_statement>(name, base_class, fields, methods);
}

std::shared_ptr<type_expression> parser::type_expr()
{
	return non_union_type();
}

std::shared_ptr<type_expression> parser::non_union_type()
{
	return generic_type();
}

std::shared_ptr<type_expression> parser::generic_type()
{
	auto name = consume(scanning::token_type::IDENTIFIER, "Type expected.");
	return make_shared<variable_type_expression>(name);
}
