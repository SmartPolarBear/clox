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
#include <interpreter/codegen/exceptions.h>

#include <interpreter/vm/opcode.h>
#include <interpreter/vm/object.h>
#include <interpreter/vm/string_object.h>

#include <ranges>

#include <gsl/gsl>

using namespace std;
using namespace gsl;

using namespace clox;
using namespace clox::scanning;
using namespace clox::parsing;
using namespace clox::resolving;
using namespace clox::interpreting;
using namespace clox::interpreting::compiling;
using namespace clox::interpreting::vm;

codegen::codegen(std::shared_ptr<vm::object_heap> heap, std::shared_ptr<resolving::binding_table> table)
		: heap_(std::move(heap)), bindings_(std::move(table))
{
	local_scopes_.push_back(make_unique<local_scope>()); // this scope may never be used.
}


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
		const std::shared_ptr<assignment_expression>& ae)
{
	if (!is_patchable(current()->peek(1)))
	{
		throw invalid_opcode(current()->peek(1));
	}

	auto unpatched = current()->peek(1);
	current()->patch(patch_main(unpatched, op_code::SET), 1);

	generate(ae->get_value());
}

void
clox::interpreting::compiling::codegen::visit_binary_expression(const std::shared_ptr<binary_expression>& be)
{
	if (bindings_->contains(be))
	{
		if (auto binding_ret = bindings_->get(be);binding_ret && binding_ret.value()->type() ==
																 resolving::binding_type::BINDING_OPERATOR)
		{
			generate(static_pointer_cast<operator_binding>(binding_ret.value())->operator_implementation_call());
			return;
		}
		else
		{
			throw internal_codegen_error{ "Invalid binding type" };
		}
	}

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

	case scanning::token_type::EQUAL_EQUAL:
		emit_byte(V(vm::op_code::EQUAL));
		break;
	case scanning::token_type::BANG_EQUAL:
		emit_byte(V(vm::op_code::EQUAL));
		emit_byte(V(vm::op_code::NOT));
		break;
	case scanning::token_type::GREATER:
		emit_byte(V(vm::op_code::GREATER));
		break;
	case scanning::token_type::GREATER_EQUAL:
		emit_byte(V(vm::op_code::GREATER_EQUAL));
		break;
	case scanning::token_type::LESS:
		emit_byte(V(vm::op_code::LESS));
		break;
	case scanning::token_type::LESS_EQUAL:
		emit_byte(V(vm::op_code::LESS_EQUAL));
		break;
	default:
		UNREACHABLE_EXCEPTION;
	}
}

void clox::interpreting::compiling::codegen::visit_unary_expression(const std::shared_ptr<unary_expression>& ue)
{
	generate(ue->get_right());


	switch (ue->get_op().type())
	{
	case scanning::token_type::MINUS:
		emit_byte(V(vm::op_code::NEGATE));
		break;
	case scanning::token_type::BANG:
		emit_byte(V(vm::op_code::NOT));
		break;

	case scanning::token_type::PLUS_PLUS:
	case scanning::token_type::MINUS_MINUS:
	{
		if (!is_patchable(current()->peek(1)))
		{
			throw invalid_opcode(current()->peek(1));
		}

		op_code op = op_code::INC;
		if (auto tt = ue->get_op().type();tt == scanning::token_type::PLUS_PLUS)
		{
			op = op_code::INC;
		}
		else if (tt == scanning::token_type::MINUS_MINUS)
		{
			op = vm::op_code::DEC;
		}

		if (!is_patchable(current()->peek(1)))
		{
			throw invalid_opcode(current()->peek(1));
		}

		auto unpatched = current()->peek(1);
		current()->patch(VC(secondary_op_code_of(unpatched) | vm::secondary_op_code::SEC_OP_PREFIX, op), 1);


		break;
	}

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
clox::interpreting::compiling::codegen::visit_postfix_expression(const std::shared_ptr<postfix_expression>& pfe)
{
	// TODO: may needs more check about if it is legal
	generate(pfe->get_left());

	switch (pfe->get_op().type())
	{
	case scanning::token_type::PLUS_PLUS:
	case scanning::token_type::MINUS_MINUS:
	{
		if (!is_patchable(current()->peek(1)))
		{
			throw invalid_opcode(current()->peek(1));
		}

		op_code op = op_code::INC;
		if (auto tt = pfe->get_op().type();tt == scanning::token_type::PLUS_PLUS)
		{
			op = op_code::INC;
		}
		else if (tt == scanning::token_type::MINUS_MINUS)
		{
			op = vm::op_code::DEC;
		}

		if (!is_patchable(current()->peek(1)))
		{
			throw invalid_opcode(current()->peek(1));
		}

		auto unpatched = current()->peek(1);
		current()->patch(VC(secondary_op_code_of(unpatched) | vm::secondary_op_code::SEC_OP_POSTFIX, op), 1);
		break;
	}
	default:
		UNREACHABLE_EXCEPTION;
	}
}

void
clox::interpreting::compiling::codegen::visit_literal_expression(const std::shared_ptr<literal_expression>& le)
{
	auto val = le->get_value();

	std::visit([this](auto&& arg)
	{
		using T = std::decay_t<decltype(arg)>;

		if constexpr(std::is_same_v<T, boolean_literal_type>)
		{
			emit_byte(V(static_cast<boolean_literal_type>(arg) ? op_code::CONSTANT_TRUE : op_code::CONSTANT_FALSE));
		}
		else if constexpr(std::is_same_v<T, nil_value_tag_type>)
		{
			emit_byte(V(vm::op_code::CONSTANT_NIL));
		}
		else if constexpr(std::is_same_v<T, string_literal_type>)
		{
			emit_constant(heap_->allocate<string_object>(arg));
		}
		else if constexpr(!std::is_same_v<T, empty_literal_tag>) // empty literal isn't meant to be a constant
		{
			emit_constant(arg);
		}
		else
		{
			return; // do nothing for empty literal
		}
	}, val);
}

void clox::interpreting::compiling::codegen::visit_grouping_expression(
		const std::shared_ptr<grouping_expression>& ge)
{
	generate(ge);
}

void clox::interpreting::compiling::codegen::visit_var_expression(const std::shared_ptr<var_expression>& ve)
{
	auto name = ve->get_name();
	auto lookup_ret = variable_lookup(name.lexeme());

	if (is_variable_lookup_failure(lookup_ret))
	{
		throw internal_codegen_error{ "Name lookup failure" };
	}

	// See opcode.h for the design here in details
	if (is_global_variable(lookup_ret))
	{
		emit_bytes(VC(SEC_OP_GLOBAL, op_code::GET), identifier_constant(name));
	}
	else
	{
		emit_bytes(VC(SEC_OP_LOCAL, op_code::GET), variable_slot(lookup_ret));
	}
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

void clox::interpreting::compiling::codegen::visit_get_expression(const std::shared_ptr<get_expression>& ge)
{

}

void clox::interpreting::compiling::codegen::visit_set_expression(const std::shared_ptr<set_expression>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_expression_statement(
		const std::shared_ptr<expression_statement>& es)
{
	generate(es->get_expr());
	emit_byte(V(op_code::POP));
}

void clox::interpreting::compiling::codegen::visit_print_statement(const std::shared_ptr<print_statement>& pe)
{
	generate(pe->get_expr());
	emit_byte(V(op_code::PRINT));
}

void
clox::interpreting::compiling::codegen::visit_variable_statement(const std::shared_ptr<variable_statement>& vs)
{
	if (vs->get_initializer())
	{
		generate(vs->get_initializer());
	}
	else
	{
		emit_byte(V(op_code::CONSTANT_NIL));
	}

	if (current_scope_depth_ == 0)
	{
		define_global_variable(vs->get_name().lexeme(), identifier_constant(vs->get_name()));
	}
	else
	{
		declare_local_variable(vs->get_name().lexeme());
	}
}

void clox::interpreting::compiling::codegen::visit_block_statement(const std::shared_ptr<block_statement>& bs)
{
	scope_begin();

	auto _ = finally([this]
	{
		scope_end();
	});

	generate(bs->get_stmts());

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

void codegen::scope_begin()
{
	current_scope_depth_++;
	local_scopes_.push_back(make_unique<local_scope>());
}

void codegen::scope_end()
{
	auto count = local_scopes_.back()->count();
	emit_bytes(
			V(op_code::POP_N),
			static_cast<chunk::code_type>(count)
	);

	local_totals_ -= count;
	local_scopes_.pop_back();
	current_scope_depth_--;
}

void codegen::define_global_variable(const string& name, vm::chunk::code_type global)
{
	local_scopes_.front()->declare(name, local_scope::GLOBAL_SLOT);
	emit_bytes(VC(SEC_OP_GLOBAL,op_code::DEFINE), global);
}

void codegen::declare_local_variable(const string& name, size_t depth)
{
	(*(local_scopes_.rbegin() + depth))->declare(name, local_totals_);
	local_totals_++;
}

uint16_t codegen::identifier_constant(const token& identifier)
{
	return make_constant(identifier.lexeme());
}

std::tuple<std::optional<vm::chunk::code_type>, bool> codegen::variable_lookup(const string& name)
{
	for (const auto& scope: local_scopes_
							| ranges::views::reverse)
	{
		if (auto find_ret = scope->find(name);find_ret)
		{
			auto slot = find_ret.value();
			if (slot == local_scope::GLOBAL_SLOT)
			{
				return make_tuple(nullopt, true);
			}
			return make_tuple(static_cast<chunk::code_type>(slot), false);
		}
	}
	return make_tuple(nullopt, false);
}

