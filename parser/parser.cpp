
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
#include <resolver/operators.h>

#include <logger/logger.h>

#include <vector>
#include <format>
#include <ranges>

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

		expr = set_parent(make_shared<binary_expression>(expr, op, right), expr, right);
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
		expr = set_parent(make_shared<ternary_expression>(expr, qmark, true_expr, colon, false_expr), expr, true_expr,
				false_expr);
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
			return set_parent(make_shared<assignment_expression>(name, val), val);
		}
		else if (expr->get_type() == PC_TYPE_get_expression)
		{
			auto ge = static_pointer_cast<get_expression>(expr);
			return set_parent(make_shared<set_expression>(ge->get_object(), ge->get_name(), val), ge->get_object(),
					val);
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
		expr = set_parent(make_shared<logical_expression>(expr, op, rhs), expr, rhs);
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
		expr = set_parent(make_shared<logical_expression>(expr, op, rhs), expr, rhs);
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

		expr = set_parent(make_shared<binary_expression>(expr, op, right), expr, right);
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

		expr = set_parent(make_shared<binary_expression>(expr, op, right), expr, right);
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
		expr = set_parent(make_shared<binary_expression>(expr, op, right), expr, right);
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
		expr = set_parent(make_shared<binary_expression>(expr, op, right), expr, right);

	}

	return expr;
}

std::shared_ptr<expression> parser::unary()
{
	if (match({ token_type::BANG, token_type::SLASH }))
	{
		auto op = previous();
		auto right = unary();

		return set_parent(make_shared<unary_expression>(op, right), right);
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
		return set_parent(make_shared<binary_expression>(expr, op, right), expr, right);
	}
	return expr;
}

std::shared_ptr<expression> parser::prefix()
{
	if (match({ token_type::PLUS_PLUS, token_type::MINUS_MINUS }))
	{
		auto op = previous();
		auto right = unary();

		return set_parent(make_shared<unary_expression>(op, right), right);
	}

	return postfix();
}

std::shared_ptr<expression> parser::postfix()
{
	auto left = lambda();

	if (match({ token_type::PLUS_PLUS, token_type::MINUS_MINUS }))
	{
		left = set_parent(make_shared<postfix_expression>(left, previous(), nullptr), left);
	}
	else if (match({ token_type::LEFT_BRACKET }))
	{
		auto lb = previous();

		auto index_expr = expr();
		left = set_parent(make_shared<postfix_expression>(left, lb, index_expr), left, index_expr);

		consume(token_type::RIGHT_BRACKET, "']' is expected after index");
	}

	return left;
}

std::shared_ptr<expression> parser::lambda()
{
	if (match({ token_type::FUN }))
	{
		auto lparen = consume(token_type::LEFT_PAREN, "'(' is expected after fun keyword in lambda expression.");

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

		consume(token_type::RIGHT_PAREN, std::format("')' is expected after lambda parameters."));

		decltype(type_expr()) ret_type{ nullptr };
		if (peek().type() == token_type::COLON)
		{
			consume(token_type::COLON, std::format("Return type is required here for lambda"));
			ret_type = type_expr();
		}

		consume(token_type::LEFT_BRACE,
				std::format("'{{' is expected after lambda declaration and before its body."));

		auto body = block(); // it will eat the '}' token

		return set_parent(make_shared<lambda_expression>(lparen, params, ret_type, body),
				params |
				views::transform([](const std::pair<clox::scanning::token, std::shared_ptr<type_expression>>& p)
				{
					return p.second;
				}), ret_type, body);
	}
	else
	{
		return call();
	}
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
			expr = set_parent(make_shared<get_expression>(expr, name), expr);
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
	return set_parent(make_shared<call_expression>(callee, paren, args), callee, args);
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

		return set_parent(make_shared<grouping_expression>(expr), expr);
	}

	throw error(peek(), "Expression is expected.");
}

token parser::consume(token_type t, const std::string& msg)
{
	// FIXME: use token parser::consume(initializer_list<scanning::token_type> tks, const string& msg)
	if (check(t))return advance();
	throw error(peek(), msg);
}

token parser::consume(initializer_list<scanning::token_type> tks, const string& msg)
{
	for (const auto& t: tks)
	{
		if (check(t))return advance();
	}
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
		auto blk = block();
		return set_parent(make_shared<block_statement>(blk), blk);
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
	return set_parent(make_shared<print_statement>(keyword, val), val);
}

std::shared_ptr<statement> parser::expr_stmt()
{
	auto e = expr();
	consume(token_type::SEMICOLON, "';' is expected after a value.");
	return set_parent(make_shared<expression_statement>(e), e);
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
			return func_declaration(function_statement_type::FST_FUNCTION);
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

token parser::func_declaration_name(function_statement_type type)
{
	if (type == function_statement_type::FST_OPERATOR)
	{
		return consume(clox::resolving::OVERRIDABLE_OPS, std::format("{} name is expected.", type));
	}
	else if (type == function_statement_type::FST_CTOR)
	{
		return previous(); // constructor itself should be its name
	}
	else
	{
		return consume(token_type::IDENTIFIER, std::format("{} name is expected.", type));
	}
}


std::shared_ptr<statement> parser::func_declaration(function_statement_type type)
{
	auto name = func_declaration_name(type);
	consume(token_type::LEFT_PAREN, std::format("'(' is expected after {} name.", type));

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

	consume(token_type::RIGHT_PAREN, std::format("')' is expected after {} name.", type));

	decltype(type_expr()) ret_type{ nullptr };
	if (peek().type() == token_type::COLON)
	{
		consume(token_type::COLON, std::format("Return type is required for callable {}", name.lexeme()));
		ret_type = type_expr();
	}

	consume(token_type::LEFT_BRACE, std::format("'{{' is expected after {} declaration and before its body.", type));

	auto body = block();

	return set_parent(make_shared<function_statement>(name, type, params, ret_type, body),
			params | views::transform([](const std::pair<clox::scanning::token, std::shared_ptr<type_expression>>& p)
			{
				return p.second;
			}), ret_type, body);
}

std::shared_ptr<statement> parser::var_statement()
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
		initializer = initializer_expr();
	}

	return set_parent(make_shared<variable_statement>(name, type, initializer), type, initializer);
}


std::shared_ptr<statement> parser::var_declaration()
{
	auto var_stmt = var_statement();
	consume(token_type::SEMICOLON, "After variable declaration, ';' is expected.");
	return var_stmt;
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

	std::optional<token> else_keyword{ nullopt };
	if (match({ token_type::ELSE }))
	{
		else_keyword = previous();
		false_stmt = stmt();
	}

	return set_parent(make_shared<if_statement>(cond, lparen, else_keyword, true_stmt, false_stmt), cond, true_stmt,
			false_stmt);
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
	return set_parent(make_shared<return_statement>(ret_keyword, val), val);
}


std::shared_ptr<statement> parser::while_stmt()
{
	auto lparen = consume(scanning::token_type::LEFT_PAREN, "'(' is expected after 'while'.");
	auto cond = this->expr();
	consume(scanning::token_type::RIGHT_PAREN, "')' is expected after 'while''s condition.");

	auto body = stmt();

	return set_parent(make_shared<while_statement>(cond, lparen, body), cond, body);
}

std::shared_ptr<statement> parser::for_stmt()
{
	auto lparen = consume(scanning::token_type::LEFT_PAREN, "'(' is expected after 'for'.");

	shared_ptr<statement> initializer{ nullptr };
	enum
	{
		INIT_NON, INIT_VAR, INIT_EXPR_STMT
	} initializer_type;

	if (match({ token_type::SEMICOLON }))
	{
		initializer_type = INIT_NON;
	}
	else if (match({ token_type::VAR }))
	{
		initializer = var_statement();
		initializer_type = INIT_VAR;
	}
	else
	{
		initializer = expr_stmt();
		initializer_type = INIT_EXPR_STMT;
	}


	if (match({ token_type::IN }))
	{
		auto in_keyword = previous();
		return foreach_finish_parse(initializer, lparen, in_keyword);
	}
	else if (match({ token_type::SEMICOLON }))
	{
		return plain_for_finish_parse(initializer, lparen);
	}

	auto token = tokens_[cur_];
	throw error(token, std::format("Wrong token \"{}\" appeared in for statement.", token));
}


std::shared_ptr<statement>
parser::foreach_finish_parse(const shared_ptr<statement>& initializer, const token& lparen, const token& in_keyword)
{
	auto iterable = expr();
	consume(scanning::token_type::RIGHT_PAREN, "')' is expected after for clauses.");

	auto body = stmt();
	auto ret = set_parent(make_shared<foreach_statement>(lparen, initializer, in_keyword, iterable, body), initializer,
			iterable, body);

	return ret;
}

std::shared_ptr<statement>
parser::plain_for_finish_parse(const shared_ptr<statement>& initializer, const clox::scanning::token& lparen)
{

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
		auto blk = vector<shared_ptr<statement>>{
				body,
				make_shared<expression_statement>(increment) };
		body = set_parent(make_shared<block_statement>(blk), blk);
	}

	if (!cond)cond = make_shared<literal_expression>(lparen, true);

	body = set_parent(make_shared<while_statement>(cond, lparen, body), cond, body);

	if (initializer)
	{
		auto blk = vector<shared_ptr<statement>>{
				initializer,
				body };
		body = set_parent(make_shared<block_statement>(blk), blk);
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

	vector<shared_ptr<variable_statement>> fields{};

	vector<shared_ptr<function_statement>> methods{};


	while (!check(scanning::token_type::RIGHT_BRACE) && !is_end())
	{
		auto [type, stmt] = class_member();
		switch (type)
		{
		case token_type::VAR:
			fields.push_back(static_pointer_cast<variable_statement>(stmt));
			break;
		case token_type::CONSTRUCTOR:
		case token_type::FUN:
		case token_type::OPERATOR:
			methods.push_back(static_pointer_cast<function_statement>(stmt));
			break;
		default:
			throw logic_error{ "type" };
		}
	}


	consume(scanning::token_type::RIGHT_BRACE, "'}' is expected after class body.");
	return make_shared<class_statement>(name, base_class,
			fields,
			methods
	);
}

std::tuple<clox::scanning::token_type, std::shared_ptr<statement>> parser::class_member()
{
	if (match({ scanning::token_type::VAR }))
	{
		return { scanning::token_type::VAR, var_declaration() };
	}
	else if (match({ scanning::token_type::FUN }))
	{
		return { scanning::token_type::FUN, func_declaration(function_statement_type::FST_METHOD) };
	}
	else if (match({ scanning::token_type::OPERATOR }))
	{
		return { scanning::token_type::OPERATOR, func_declaration(function_statement_type::FST_OPERATOR) };
	}
	else if (match({ scanning::token_type::CONSTRUCTOR }))
	{
		return { scanning::token_type::CONSTRUCTOR, func_declaration(function_statement_type::FST_CTOR) };
	}
	else
	{
		throw error(peek(), "Field or method or operator is expected.");
	}
}

std::shared_ptr<type_expression> parser::type_expr()
{
	auto left = non_union_type();
	while (match({ token_type::PIPE }))
	{
		auto pipe = previous();
		auto right = non_union_type();
		left = make_shared<union_type_expression>(left, pipe, right);
	}

	return left;
}

std::shared_ptr<type_expression> parser::non_union_type()
{
	shared_ptr<type_expression> type{ nullptr };
	if (match({ token_type::FUN }))
	{
		type = callable_type();
	}
	else if (match({ token_type::LIST }))
	{
		auto bracket = consume(token_type::LEFT_BRACKET, "Type specifier [type] is required after a list");
		auto element_type = type_expr();
		consume(token_type::RIGHT_BRACKET, "Missing ']' for type specifier of list ");
		type = make_shared<list_type_expression>(element_type, bracket);
	}
	else if (match({ token_type::MAP }))
	{
		auto bracket = consume(token_type::LEFT_BRACKET, "Type specifier [type] is required after a map");
		auto key_type = type_expr();
		auto comma = consume(token_type::COMMA, "A comma is required between key and value type.");
		auto val_type = type_expr();
		consume(token_type::RIGHT_BRACKET, "Missing ']' for type specifier of map ");
		type = make_shared<map_type_expression>(key_type, val_type, bracket, comma);
	}
	else
	{
		type = variable_type();
	}


	return type;
}


std::shared_ptr<type_expression> parser::callable_type()
{
	auto fun_token = previous();

	auto lparen = consume(token_type::LEFT_PAREN, "'(' is expected after 'fun'");

	vector<shared_ptr<type_expression>> param_types;
	if (!check(token_type::RIGHT_PAREN))
	{
		do
		{
			param_types.push_back(type_expr());
		} while (match({ token_type::COMMA }));
	}

	auto rparen = consume(token_type::RIGHT_PAREN, "')' is expected after parameter types");

	shared_ptr<type_expression> return_type{ nullptr };
	if (match({ token_type::COLON }))
	{
		return_type = type_expr();
	}

	return make_shared<callable_type_expression>(fun_token, lparen, param_types, rparen, return_type);
}


std::shared_ptr<type_expression> parser::variable_type()
{
	auto name = consume(scanning::token_type::IDENTIFIER, "Type name expected.");
	return make_shared<variable_type_expression>(name);
}

std::shared_ptr<expression> parser::initializer_list_expr()
{
	auto keyword = previous();
	consume(token_type::LEFT_BRACE, "Initializer list must start with '}'");

	vector<shared_ptr<expression>> init_items{ initializer_expr() };
	while (match({ token_type::COMMA }))
	{
		init_items.push_back(initializer_expr());
	}

	while (match({ token_type::COMMA })); // Do nothing ( consume trailing commas like {1,2,3,,,} )

	consume(token_type::RIGHT_BRACE, "Initializer list must be enclosed by '}'");

	return make_shared<list_initializer_expression>(keyword, init_items);
}

std::shared_ptr<expression> parser::map_initializer_list_expr()
{
	auto keyword = previous();
	consume(token_type::LEFT_BRACE, "Initializer list must start with '}'");

	vector<pair<shared_ptr<expression>, shared_ptr<expression>>> init_items{};

	init_items.emplace_back(initializer_expr(), initializer_expr());

	while (match({ token_type::COMMA }))
	{
		auto first = initializer_expr(), second = initializer_expr();
		init_items.emplace_back(first, second);
	}

	while (match({ token_type::COMMA })); // Do nothing ( consume trailing commas like {1,2,3,,,} )

	consume(token_type::RIGHT_BRACE, "Initializer list must be enclosed by '}'");

	return make_shared<map_initializer_expression>(keyword, init_items);
}


std::shared_ptr<expression> parser::initializer_expr()
{
	if (match({ token_type::LIST }))
	{
		return initializer_list_expr();
	}
	else if (match({ token_type::MAP }))
	{
		return map_initializer_list_expr();
	}

	return conditional();
}

