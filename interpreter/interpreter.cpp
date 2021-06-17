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

//
// Created by cleve on 6/14/2021.
//

#include <scanner/scanner.h>

#include <interpreter/interpreter.h>
#include <interpreter/runtime_error.h>

#include <logger/logger.h>

#include <iostream>

#include <utility>

using namespace std;
using namespace clox::scanning;
using namespace clox::parsing;
using namespace clox::interpreting;


clox::interpreting::interpreting_result
clox::interpreting::interpreter::visit_binary_expression(const std::shared_ptr<binary_expression>& be)
{
	auto left = evaluate(be->get_left());
	auto right = evaluate(be->get_right());

	switch (be->get_op().type())
	{
	case scanning::token_type::GREATER:
		check_numeric_operands(be->get_op(), left, right);
		return get<long double>(left) > get<long double>(right);
	case scanning::token_type::GREATER_EQUAL:
		check_numeric_operands(be->get_op(), left, right);
		return get<long double>(left) >= get<long double>(right);
	case scanning::token_type::LESS:
		check_numeric_operands(be->get_op(), left, right);
		return get<long double>(left) < get<long double>(right);
	case scanning::token_type::LESS_EQUAL:
		check_numeric_operands(be->get_op(), left, right);
		return get<long double>(left) <= get<long double>(right);

	case scanning::token_type::MINUS:
		check_numeric_operands(be->get_op(), left, right);
		return get<long double>(left) - get<long double>(right);
	case scanning::token_type::PLUS:
		if (holds_alternative<long double>(left) && holds_alternative<long double>(right))
		{
			return get<long double>(left) + get<long double>(right);
		}
		else if (holds_alternative<string>(left) && holds_alternative<string>(right))
		{
			return get<string>(left) + get<string>(right);
		}

		throw clox::interpreting::runtime_error(be->get_op(), "Operands should be strings or numbers.");
	case scanning::token_type::SLASH:
		check_numeric_operands(be->get_op(), left, right);
		return get<long double>(left) / get<long double>(right);
	case scanning::token_type::STAR:
		check_numeric_operands(be->get_op(), left, right);
		return get<long double>(left) * get<long double>(right);


	case scanning::token_type::BANG_EQUAL:
		return !is_equal(left, right);
	case scanning::token_type::EQUAL_EQUAL:
		return is_equal(left, right);

	default:
		//TODO: ERROR?
		break;
	}

	// Should not reach here
	return nil_value_tag;
}

clox::interpreting::interpreting_result
clox::interpreting::interpreter::visit_unary_expression(const std::shared_ptr<unary_expression>& ue)
{
	auto right = evaluate(ue->get_right());

	switch (ue->get_op().type())
	{
	case scanning::token_type::MINUS:
		return -get<long double>(right);
		break;
	case scanning::token_type::BANG:
		return !is_truthy(right);
	default:
		//TODO: ERROR?
		break;
	}

	// Should not reach here
	return nil_value_tag;
}

clox::interpreting::interpreting_result
clox::interpreting::interpreter::visit_literal(const std::shared_ptr<literal>& literal)
{
	return interpreter::literal_value_to_interpreting_result(literal->get_value());
}

clox::interpreting::interpreting_result
clox::interpreting::interpreter::visit_grouping(const std::shared_ptr<grouping>& grouping)
{
	return evaluate(grouping->get_expr());
}

std::string clox::interpreting::interpreter::result_to_string(const clox::interpreting::interpreting_result& res)
{
	if (holds_alternative<nil_value_tag_type>(res))return "nil";

	if (holds_alternative<long double>(res))
	{
		return std::to_string(get<long double>(res));
	}
	else if (holds_alternative<bool>(res))
	{
		return string{ bool_to_string(get<bool>(res)) };
	}
	else if (holds_alternative<string>(res))
	{
		return get<string>(res);
	}

	// FIXME: error?
	return "";
}

void clox::interpreting::interpreter::interpret(const shared_ptr<expression>& expr)
{
	try
	{
		auto val = evaluate(expr);
		cout << result_to_string(val) << endl;
	}
	catch (const clox::interpreting::runtime_error& re)
	{
		clox::logging::logger::instance().runtime_error(re);
	}
}

clox::interpreting::interpreting_result
clox::interpreting::interpreter::literal_value_to_interpreting_result(const literal_value_type& value)
{
	if (holds_alternative<long double>(value))
	{
		return get<long double>(value);
	}
	else if (holds_alternative<bool>(value))
	{
		return get<bool>(value);
	}
	else if (holds_alternative<string>(value))
	{
		return get<std::string>(value);
	}
	else if (holds_alternative<nil_value_tag_type>(value))
	{
		return get<scanning::nil_value_tag_type>(value);
	}
	else
	{
		throw invalid_argument("value");
	}
}

clox::interpreting::interpreting_result clox::interpreting::interpreter::evaluate(const shared_ptr<expression>& expr)
{
	return accept(*expr, *this);
}


bool clox::interpreting::interpreter::is_truthy(clox::interpreting::interpreting_result e)
{
	// Ruby’s rule: false and nil are false, and everything else is truthy
	if (holds_alternative<nil_value_tag_type>(e))return false;
	if (holds_alternative<bool>(e))return get<bool>(e);
	return true;
}

bool clox::interpreting::interpreter::is_equal(clox::interpreting::interpreting_result lhs,
		clox::interpreting::interpreting_result rhs)
{
	if (holds_alternative<nil_value_tag_type>(lhs) && holds_alternative<nil_value_tag_type>(rhs))
	{
		return true;
	}
	else if (holds_alternative<nil_value_tag_type>(lhs) || holds_alternative<nil_value_tag_type>(rhs))
	{
		return false;
	}

	if (holds_alternative<long double>(lhs))
	{
		return get<long double>(lhs) == get<long double>(rhs);
	}
	else if (holds_alternative<string>(lhs))
	{
		return get<string>(lhs) == get<string>(rhs);
	}
	else if (holds_alternative<bool>(lhs))
	{
		return get<bool>(lhs) == get<bool>(rhs);
	}

	//FIXME :error?
	return false;
}

void clox::interpreting::interpreter::check_numeric_operands(token op, const clox::interpreting::interpreting_result& l,
		const clox::interpreting::interpreting_result& r)
{
	if (holds_alternative<long double>(l) && holds_alternative<long double>(r))return;

	throw clox::interpreting::runtime_error(std::move(op), "Operands must be numbers.");
}

constexpr std::string_view interpreter::bool_to_string(bool b)
{
	return b ? "true" : "false";
}
