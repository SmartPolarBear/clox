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
// Created by cleve on 6/12/2021.
//

#include <parser/ast_printer.h>
#include <parser/parser.h>

#include <sstream>
#include <format>

using namespace std;

using namespace clox::parsing;
using namespace clox::scanning;

std::string clox::parsing::ast_printer::visit_binary_expression(const std::shared_ptr<binary_expression>& e)
{
	return parenthesize(any_cast<token>(e->get_op()).lexeme(),
			{ e->get_left().get(), e->get_right().get() });
}

std::string clox::parsing::ast_printer::visit_unary_expression(const std::shared_ptr<unary_expression>& e)
{
	return parenthesize(any_cast<token>(e->get_op()).lexeme(),
			{ e->get_right().get() });
}

std::string clox::parsing::ast_printer::visit_literal_expression(const std::shared_ptr<literal_expression>& literal)
{
	if (holds_alternative<nil_value_tag_type>(literal->get_value()))
	{
		return "nil";
	}
	else if (holds_alternative<string>(literal->get_value()))
	{
		return std::format("<literal>({0})={1}", "string", get<string>(literal->get_value()));
	}
	else if (holds_alternative<long double>(literal->get_value()))
	{
		return std::format("<literal>({})", get<long double>(literal->get_value()));
	}
	else if (holds_alternative<bool>(literal->get_value()))
	{
		return std::format("<literal>({})", get<bool>(literal->get_value()));
	}

	return std::format("<literal>({})", "Unknown");
}

std::string clox::parsing::ast_printer::visit_grouping_expression(const shared_ptr<grouping_expression>& expression)
{
	return parenthesize("group", { expression.get() });
}

std::string ast_printer::to_string(const expression& expr)
{
	return accept(expr, *this);
}

std::string ast_printer::parenthesize(const std::string& name, initializer_list<expression*> exprs)
{
	stringstream ss{};

	ss << "(" << name;
	for (auto& expr:exprs)
	{
		ss << " " << accept(*expr, *this);
	}

	ss << ")";
	return ss.str();
}
