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

#include <helper/exceptions.h>

#include <interpreter/codegen/codegen.h>
#include <interpreter/vm/opcode.h>

#include <gsl/gsl>



#ifdef V
#warning "V is already defined"
#undef V
#endif

using namespace std;
using namespace gsl;

using namespace clox;
using namespace clox::scanning;
using namespace clox::parsing;
using namespace clox::interpreting;
using namespace clox::interpreting::compiling;
using namespace clox::interpreting::vm;

#define V(op) op_code_value(op)

void codegen::generate(const std::shared_ptr<parsing::statement>& s)
{
	accept(*s, *dynamic_cast<statement_visitor<void>*>(this));
}

void codegen::generate(const shared_ptr<parsing::expression>& e)
{
	accept(*e, *dynamic_cast<expression_visitor<void>*>(this));
}

void codegen::generate(const vector<std::shared_ptr<parsing::statement>>& stmts)
{
	for (const auto& stmt: stmts)
	{
		generate(stmt);
	}

	emit_return();
}

void clox::interpreting::compiling::codegen::visit_assignment_expression(
		const std::shared_ptr<assignment_expression>& ptr)
{

}

void
clox::interpreting::compiling::codegen::visit_binary_expression(const std::shared_ptr<binary_expression>& be)
{
	generate(be->get_left());

	generate(be->get_right());

	switch (be->get_op().type())
	{
	case scanning::token_type::PLUS:
		emit_byte(V(vm::op_code::ADD));
		break;
	case scanning::token_type::MINUS:
		emit_byte(V(vm::op_code::SUBTRACT));
		break;
	case scanning::token_type::SLASH:
		emit_byte(V(vm::op_code::DIVIDE));
		break;
	case scanning::token_type::STAR:
		emit_byte(V(vm::op_code::MULTIPLY));
		break;
	case scanning::token_type::STAR_STAR:
		emit_byte(V(vm::op_code::POW));
		break;
	default:
		UNREACHABLE_EXCEPTION;
	}
}

void clox::interpreting::compiling::codegen::visit_unary_expression(const std::shared_ptr<unary_expression>& ue)
{
	generate(ue);

	switch (ue->get_op().type())
	{
	case scanning::token_type::MINUS:
		emit_byte(V(vm::op_code::NEGATE));
		break;
	default:
		UNREACHABLE_EXCEPTION;
	}
}

void clox::interpreting::compiling::codegen::visit_this_expression(const std::shared_ptr<this_expression>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_base_expression(const std::shared_ptr<base_expression>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_initializer_list_expression(
		const std::shared_ptr<initializer_list_expression>& ptr)
{

}

void
clox::interpreting::compiling::codegen::visit_postfix_expression(const std::shared_ptr<postfix_expression>& ptr)
{

}

void
clox::interpreting::compiling::codegen::visit_literal_expression(const std::shared_ptr<literal_expression>& le)
{
	auto val = le->get_value();
	std::visit([this](auto&& arg)
	{
		using T = std::decay_t<decltype(arg)>;
		if constexpr(!std::is_same_v<T, empty_literal_tag>) // empty literal isn't meant to be a constant
		{
			emit_constant(arg);
		}
	}, val);
}

void clox::interpreting::compiling::codegen::visit_grouping_expression(
		const std::shared_ptr<grouping_expression>& ge)
{
	generate(ge);
}

void clox::interpreting::compiling::codegen::visit_var_expression(const std::shared_ptr<var_expression>& ptr)
{

}

void
clox::interpreting::compiling::codegen::visit_ternary_expression(const std::shared_ptr<ternary_expression>& ptr)
{

}

void
clox::interpreting::compiling::codegen::visit_logical_expression(const std::shared_ptr<logical_expression>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_call_expression(const std::shared_ptr<call_expression>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_get_expression(const std::shared_ptr<get_expression>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_set_expression(const std::shared_ptr<set_expression>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_expression_statement(
		const std::shared_ptr<expression_statement>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_print_statement(const std::shared_ptr<print_statement>& ptr)
{

}

void
clox::interpreting::compiling::codegen::visit_variable_statement(const std::shared_ptr<variable_statement>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_block_statement(const std::shared_ptr<block_statement>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_while_statement(const std::shared_ptr<while_statement>& ptr)
{

}

void
clox::interpreting::compiling::codegen::visit_foreach_statement(const std::shared_ptr<foreach_statement>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_if_statement(const std::shared_ptr<if_statement>& ptr)
{

}

void
clox::interpreting::compiling::codegen::visit_function_statement(const std::shared_ptr<function_statement>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_return_statement(const std::shared_ptr<return_statement>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_class_statement(const std::shared_ptr<class_statement>& ptr)
{

}

std::shared_ptr<vm::chunk> codegen::current()
{
	return current_chunk_;
}

void codegen::emit_byte(uint16_t byte)
{
	current()->write(byte);
}


void codegen::emit_return()
{
	emit_byte(V(op_code::RETURN));
}

void codegen::emit_constant(const value& val)
{
	emit_bytes(V(op_code::CONSTANT), make_constant(val));
}

uint16_t codegen::make_constant(const value& val)
{
	auto idx = current()->add_constant(val);
	return idx;
}

