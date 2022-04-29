// Copyright (c) 2022 SmartPolarBear
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
// Created by cleve on 4/29/2022.
//

#include <scoper/scoper.h>

using namespace clox::parsing;

bool clox::resolving::scoping::scoper::visit_assignment_expression(const std::shared_ptr<assignment_expression>& expr)
{
	auto is_const = resolve(expr->get_value());


}

bool clox::resolving::scoping::scoper::visit_binary_expression(const std::shared_ptr<binary_expression>& ptr)
{
	return false;
}
bool clox::resolving::scoping::scoper::visit_unary_expression(const std::shared_ptr<unary_expression>& ptr)
{
	return false;
}
bool clox::resolving::scoping::scoper::visit_this_expression(const std::shared_ptr<this_expression>& ptr)
{
	return false;
}
bool clox::resolving::scoping::scoper::visit_base_expression(const std::shared_ptr<base_expression>& ptr)
{
	return false;
}
bool clox::resolving::scoping::scoper::visit_postfix_expression(const std::shared_ptr<postfix_expression>& ptr)
{
	return false;
}
bool clox::resolving::scoping::scoper::visit_literal_expression(const std::shared_ptr<literal_expression>& ptr)
{
	return false;
}
bool clox::resolving::scoping::scoper::visit_grouping_expression(const std::shared_ptr<grouping_expression>& ptr)
{
	return false;
}
bool clox::resolving::scoping::scoper::visit_lambda_expression(const std::shared_ptr<lambda_expression>& ptr)
{
	return false;
}
bool clox::resolving::scoping::scoper::visit_var_expression(const std::shared_ptr<var_expression>& ptr)
{
	return false;
}
bool clox::resolving::scoping::scoper::visit_ternary_expression(const std::shared_ptr<ternary_expression>& ptr)
{
	return false;
}
bool clox::resolving::scoping::scoper::visit_logical_expression(const std::shared_ptr<logical_expression>& ptr)
{
	return false;
}
bool clox::resolving::scoping::scoper::visit_call_expression(const std::shared_ptr<call_expression>& ptr)
{
	return false;
}
bool clox::resolving::scoping::scoper::visit_list_initializer_expression(const std::shared_ptr<
	list_initializer_expression>& ptr)
{
	return false;
}
bool clox::resolving::scoping::scoper::visit_map_initializer_expression(const std::shared_ptr<map_initializer_expression>& ptr)
{
	return false;
}
bool clox::resolving::scoping::scoper::visit_get_expression(const std::shared_ptr<get_expression>& ptr)
{
	return false;
}
bool clox::resolving::scoping::scoper::visit_set_expression(const std::shared_ptr<set_expression>& ptr)
{
	return false;
}
void clox::resolving::scoping::scoper::visit_expression_statement(const std::shared_ptr<expression_statement>& ptr)
{

}
void clox::resolving::scoping::scoper::visit_print_statement(const std::shared_ptr<print_statement>& ptr)
{

}
void clox::resolving::scoping::scoper::visit_variable_statement(const std::shared_ptr<variable_statement>& ptr)
{

}
void clox::resolving::scoping::scoper::visit_block_statement(const std::shared_ptr<block_statement>& ptr)
{

}
void clox::resolving::scoping::scoper::visit_while_statement(const std::shared_ptr<while_statement>& ptr)
{

}
void clox::resolving::scoping::scoper::visit_foreach_statement(const std::shared_ptr<foreach_statement>& ptr)
{

}
void clox::resolving::scoping::scoper::visit_if_statement(const std::shared_ptr<if_statement>& ptr)
{

}
void clox::resolving::scoping::scoper::visit_function_statement(const std::shared_ptr<function_statement>& ptr)
{

}
void clox::resolving::scoping::scoper::visit_return_statement(const std::shared_ptr<return_statement>& ptr)
{

}
void clox::resolving::scoping::scoper::visit_class_statement(const std::shared_ptr<class_statement>& ptr)
{

}
void clox::resolving::scoping::scoper::visit_variable_type_expression(const std::shared_ptr<variable_type_expression>& ptr)
{

}
void clox::resolving::scoping::scoper::visit_union_type_expression(const std::shared_ptr<union_type_expression>& ptr)
{

}
void clox::resolving::scoping::scoper::visit_list_type_expression(const std::shared_ptr<list_type_expression>& ptr)
{

}
void clox::resolving::scoping::scoper::visit_map_type_expression(const std::shared_ptr<map_type_expression>& ptr)
{

}
void clox::resolving::scoping::scoper::visit_callable_type_expression(const std::shared_ptr<callable_type_expression>& ptr)
{

}

void clox::resolving::scoping::scoper::resolve(const std::vector<std::shared_ptr<parsing::statement>>& stmts)
{
	for (const auto& stmt : stmts)
	{
		resolve(stmt);
	}
}

void clox::resolving::scoping::scoper::resolve(const std::shared_ptr<parsing::statement>& stmt)
{
	accept(*stmt, *this);
}

void clox::resolving::scoping::scoper::resolve(const std::shared_ptr<parsing::type_expression>& expr)
{
	accept(*expr, *this);
}

bool clox::resolving::scoping::scoper::resolve(const std::shared_ptr<parsing::expression>& expr)
{
	return accept(*expr, *this);
}
