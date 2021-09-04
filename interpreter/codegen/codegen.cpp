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
// Created by cleve on 9/4/2021.
//

#include <interpreter/codegen.h>

#include <gsl/gsl>

using namespace std;
using namespace gsl;

using namespace clox;
using namespace clox::scanning;
using namespace clox::parsing;
using namespace clox::interpreting;
using namespace clox::interpreting::compiling;

void clox::interpreting::compiling::codegen::visit_assignment_expression(
		const std::shared_ptr< assignment_expression>& ptr)
{

}

void
clox::interpreting::compiling::codegen::visit_binary_expression(const std::shared_ptr< binary_expression>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_unary_expression(const std::shared_ptr< unary_expression>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_this_expression(const std::shared_ptr< this_expression>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_base_expression(const std::shared_ptr< base_expression>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_initializer_list_expression(
		const std::shared_ptr< initializer_list_expression>& ptr)
{

}

void
clox::interpreting::compiling::codegen::visit_postfix_expression(const std::shared_ptr< postfix_expression>& ptr)
{

}

void
clox::interpreting::compiling::codegen::visit_literal_expression(const std::shared_ptr< literal_expression>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_grouping_expression(
		const std::shared_ptr< grouping_expression>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_var_expression(const std::shared_ptr< var_expression>& ptr)
{

}

void
clox::interpreting::compiling::codegen::visit_ternary_expression(const std::shared_ptr< ternary_expression>& ptr)
{

}

void
clox::interpreting::compiling::codegen::visit_logical_expression(const std::shared_ptr< logical_expression>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_call_expression(const std::shared_ptr< call_expression>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_get_expression(const std::shared_ptr< get_expression>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_set_expression(const std::shared_ptr< set_expression>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_expression_statement(
		const std::shared_ptr< expression_statement>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_print_statement(const std::shared_ptr< print_statement>& ptr)
{

}

void
clox::interpreting::compiling::codegen::visit_variable_statement(const std::shared_ptr< variable_statement>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_block_statement(const std::shared_ptr< block_statement>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_while_statement(const std::shared_ptr< while_statement>& ptr)
{

}

void
clox::interpreting::compiling::codegen::visit_foreach_statement(const std::shared_ptr< foreach_statement>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_if_statement(const std::shared_ptr< if_statement>& ptr)
{

}

void
clox::interpreting::compiling::codegen::visit_function_statement(const std::shared_ptr< function_statement>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_return_statement(const std::shared_ptr< return_statement>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_class_statement(const std::shared_ptr< class_statement>& ptr)
{

}
