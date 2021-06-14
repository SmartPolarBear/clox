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

#include <parser/interpreter.h>

using namespace std;
using namespace clox::scanning;

clox::parsing::interpreting_result
clox::parsing::interpreter::visit_binary_expression(const std::shared_ptr<binary_expression>& be)
{
	auto left = evaluate(be->get_left());
	auto right = evaluate(be->get_right());

	switch (be->get_op().type())
	{
	case scanning::token_type::MINUS:
	case scanning::token_type::SLASH:
	case scanning::token_type::STAR:
	case scanning::token_type::PLUS:


	case scanning::token_type::GREATER:
	case scanning::token_type::GREATER_EQUAL:
	case scanning::token_type::LESS:
	case scanning::token_type::LESS_EQUAL:

	case scanning::token_type::BANG_EQUAL:
	case scanning::token_type::EQUAL_EQUAL:


	default:
		//TODO: ERROR?
		break;
	}

	// Should not reach here
	return nil_value_tag;
}

clox::parsing::interpreting_result
clox::parsing::interpreter::visit_unary_expression(const std::shared_ptr<unary_expression>& ue)
{
	auto right = evaluate(ue->get_right());

	switch (ue->get_op().type())
	{
	case scanning::token_type::MINUS:
		return -get<long double>(right);
		break;
	case scanning::token_type::BANG:
		return !is_truth(right);
	default:
		//TODO: ERROR?
		break;
	}

	// Should not reach here
	return nil_value_tag;
}

clox::parsing::interpreting_result clox::parsing::interpreter::visit_literal(const std::shared_ptr<literal>& literal)
{
	return interpreter::literal_value_to_interpreting_result(literal->get_value());
}

clox::parsing::interpreting_result clox::parsing::interpreter::visit_grouping(const std::shared_ptr<grouping>& grouping)
{
	return evaluate(grouping->get_expr());
}

std::string clox::parsing::interpreter::result_to_string(const clox::parsing::interpreting_result& res)
{
	return std::string();
}

void clox::parsing::interpreter::interpret(const clox::parsing::expression& expr)
{

}

clox::parsing::interpreting_result clox::parsing::interpreter::literal_value_to_interpreting_result(std::any any)
{
	if (any.type() == typeid(long double))
	{
		return any_cast<long double>(any);
	}
	else if (any.type() == typeid(bool))
	{
		return any_cast<bool>(any);
	}
	else if (any.type() == typeid(std::string))
	{
		return any_cast<std::string>(any);
	}
	else if (any.type() == typeid(scanning::nil_value_tag_type))
	{
		return any_cast<scanning::nil_value_tag_type>(any);
	}
	else
	{
		throw invalid_argument("any");
	}
}

clox::parsing::interpreting_result clox::parsing::interpreter::evaluate(const shared_ptr<expression>& expr)
{
	return clox::parsing::interpreting_result();
}

bool clox::parsing::interpreter::is_truth(clox::parsing::interpreting_result e)
{
	if (holds_alternative<nil_value_tag_type>(e))return false;
	if (holds_alternative<bool>(e))return get<bool>(e);
	return true;
}
