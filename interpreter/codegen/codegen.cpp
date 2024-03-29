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
#include <helper/std_console.h>

#include <interpreter/codegen/codegen.h>
#include <interpreter/codegen/exceptions.h>
#include <interpreter/codegen/exceptions.h>

#include <interpreter/vm/opcode.h>
#include "object/object.h"
#include "object/string_object.h"
#include <interpreter/vm/exceptions.h>

#include <ranges>

#include <gsl/gsl>
#include "resolver/ast_annotation.h"

using namespace std;
using namespace gsl;

using namespace clox;
using namespace clox::scanning;
using namespace clox::parsing;
using namespace clox::resolving;
using namespace clox::interpreting;
using namespace clox::interpreting::compiling;
using namespace clox::interpreting::vm;

codegen::codegen(std::shared_ptr<vm::object_heap> heap, const resolving::resolver& rsv)
	: heap_(std::move(heap)), scopes_(rsv.global_scope()), scope_iterator_(scopes_.begin()), resolver_(&rsv)
{
	function_push(heap_->allocate<function_object>("", 0));
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
	for (const auto& stmt : stmts)
	{
		generate(stmt);
	}
}

void codegen::scope_begin()
{
	scope_iterator_++;

	if (auto scope = *scope_iterator_;scope->scope_type() == resolving::scope_types::FUNCTION_SCOPE)
	{
		auto func = static_pointer_cast<function_scope>(scope);
		function_top()->upvalue_count_ = func->upvalues().size();
		emit_code(func->upvalues().size());
		for (const auto& upval : func->upvalues())
		{
			emit_codes(upval->holds_symbol() /*it is a local variable */, upval->access_index());
		}
	}
}

void codegen::scope_end()
{
	auto scope = *scope_iterator_;

	if (scope->scope_type() == resolving::scope_types::FUNCTION_SCOPE)
	{
		for (auto& var : scope->names())
		{
			auto named = static_pointer_cast<named_symbol>(var.second);
			if (named->is_captured())
			{
				emit_code(V(op_code::CLOSE_UPVALUE));
			}
			else
			{
				emit_code(V(op_code::POP));
			}
		}
	}
	else if (scope->scope_type() == resolving::scope_types::CLASS_BASE_SCOPE ||
		scope->scope_type() == resolving::scope_types::CLASS_FIELD_SCOPE)
	{
		// Do nothing on purpose
	}
	else
	{
		emit_codes(
			V(op_code::POP_N),
			static_cast<chunk::code_type>(scope->names().size())
		);
	}

	scope_iterator_--;
}

std::shared_ptr<resolving::scope> codegen::current_scope()
{
	return *scope_iterator_;
}

void clox::interpreting::compiling::codegen::visit_assignment_expression(
	const std::shared_ptr<assignment_expression>& ae)
{
//	generate(ae->get_value());

//	if (!is_patchable(current()->peek(1)))
//	{
//		current()->disassemble(helper::std_console::instance());
//		throw invalid_opcode(current()->peek(1));
//	}
//
//	auto unpatched = current()->peek(1);
//	current()->patch(patch_main(unpatched, op_code::SET), 1);
//

// The above algorithm will be useful if we unify the assignment expression and set expression

	generate(ae->get_value());

	auto name = ae->get_name();
	auto binding = variable_lookup(ae);

	if (!binding)
	{
		throw internal_codegen_error{ "Name lookup failure" };
	}

	if (auto upvalue = binding->upvalue();upvalue)
	{
		emit_codes(ae->get_name(), VC(SEC_OP_UPVALUE, op_code::SET), upvalue->current_index());
	}
	else
	{
		auto symbol = binding->symbol();

		// See opcode.h for the design here in details
		if (symbol->is_global())
		{
			emit_codes(ae->get_name(), VC(SEC_OP_GLOBAL, op_code::SET), identifier_constant(name));
		}
		else
		{
			emit_codes(ae->get_name(), VC(SEC_OP_LOCAL, op_code::SET), symbol->slot_index());
		}
	}
}

void
clox::interpreting::compiling::codegen::visit_binary_expression(const std::shared_ptr<binary_expression>& be)
{
	if (auto annotation = be->get_annotation<operator_annotation>();annotation)
	{
		generate(annotation->operator_implementation_call());
		return;
	}

	generate(be->get_left());

	generate(be->get_right());

	switch (be->get_op().type())
	{
	case scanning::token_type::PLUS:
		emit_code(be->get_op(), V(vm::op_code::ADD));
		break;
	case scanning::token_type::MINUS:
		emit_code(be->get_op(), V(vm::op_code::SUBTRACT));
		break;
	case scanning::token_type::SLASH:
		emit_code(be->get_op(), V(vm::op_code::DIVIDE));
		break;
	case scanning::token_type::STAR:
		emit_code(be->get_op(), V(vm::op_code::MULTIPLY));
		break;
	case scanning::token_type::STAR_STAR:
		emit_code(be->get_op(), V(vm::op_code::POW));
		break;

	case scanning::token_type::EQUAL_EQUAL:
		emit_code(be->get_op(), V(vm::op_code::EQUAL));
		break;
	case scanning::token_type::BANG_EQUAL:
		emit_code(be->get_op(), V(vm::op_code::EQUAL));
		emit_code(be->get_op(), V(vm::op_code::NOT));
		break;
	case scanning::token_type::GREATER:
		emit_code(be->get_op(), V(vm::op_code::GREATER));
		break;
	case scanning::token_type::GREATER_EQUAL:
		emit_code(be->get_op(), V(vm::op_code::GREATER_EQUAL));
		break;
	case scanning::token_type::LESS:
		emit_code(be->get_op(), V(vm::op_code::LESS));
		break;
	case scanning::token_type::LESS_EQUAL:
		emit_code(be->get_op(), V(vm::op_code::LESS_EQUAL));
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
		emit_code(ue->get_op(), V(vm::op_code::NEGATE));
		break;
	case scanning::token_type::BANG:
		emit_code(ue->get_op(), V(vm::op_code::NOT));
		break;

	case scanning::token_type::PLUS_PLUS:
	case scanning::token_type::MINUS_MINUS:
	{
		if (!is_patchable(current_chunk()->peek(1)))
		{
			throw invalid_opcode(current_chunk()->peek(1));
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

		if (!is_patchable(current_chunk()->peek(1)))
		{
			throw invalid_opcode(current_chunk()->peek(1));
		}

		auto unpatched = current_chunk()->peek(1);
		current_chunk()->patch_end(VC(secondary_op_code_of(unpatched) | vm::secondary_op_code::SEC_OP_PREFIX, op), 1);

		break;
	}

	default:
		UNREACHABLE_EXCEPTION;
	}
}

void clox::interpreting::compiling::codegen::visit_this_expression(const std::shared_ptr<this_expression>& te)
{
	emit_codes(te->get_keyword(), VC(SEC_OP_LOCAL, op_code::GET), 0);
}

void clox::interpreting::compiling::codegen::visit_base_expression(const std::shared_ptr<base_expression>& be)
{
	emit_codes(be->get_keyword(), VC(SEC_OP_LOCAL, op_code::GET), 0); // get this object

	auto annotation = be->get_annotation<base_annotation>();
	if (annotation->field_type() == resolving::base_annotation::base_field_type::FIELD)
	{
		emit_codes(VC(SEC_OP_LOCAL, vm::op_code::GET_SUPER), annotation->index(), annotation->field_id());
	}
	else if (annotation->field_type() == resolving::base_annotation::base_field_type::METHOD)
	{
		emit_codes(VC(SEC_OP_FUNC, vm::op_code::GET_SUPER), annotation->index(), annotation->field_id());
	}
}

void
clox::interpreting::compiling::codegen::visit_postfix_expression(const std::shared_ptr<postfix_expression>& pfe)
{
	// TODO: may needs more check about if it is legal
	generate(pfe->get_left());

	switch (pfe->get_op().type())
	{
	case scanning::token_type::LEFT_BRACKET:
	{
		generate(pfe->get_optional_right());

		auto annotation = pfe->get_annotation<container_annotation>();
		assert(annotation);
		switch (annotation->container_type())
		{
		case container_annotation::container_types::LIST:
			emit_code(pfe->get_op(), V(op_code::LIST_ELEM));
			break;
		case container_annotation::container_types::MAP:
			emit_code(pfe->get_op(), V(op_code::MAP_ELEM));
			break;
		}
		break;
	}
	case scanning::token_type::PLUS_PLUS:
	case scanning::token_type::MINUS_MINUS:
	{
		if (!is_patchable(current_chunk()->peek(1)))
		{
			throw invalid_opcode(current_chunk()->peek(1));
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

		if (!is_patchable(current_chunk()->peek(1)))
		{
			throw invalid_opcode(current_chunk()->peek(1));
		}

		auto unpatched = current_chunk()->peek(1);
		current_chunk()->patch_end(VC(secondary_op_code_of(unpatched) | vm::secondary_op_code::SEC_OP_POSTFIX, op), 1);
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

	std::visit([this, &le](auto&& arg)
	{
	  using T = std::decay_t<decltype(arg)>;

	  if constexpr(std::is_same_v<T, boolean_literal_type>)
	  {
		  emit_code(le->get_token(),
			  V(static_cast<boolean_literal_type>(arg) ? op_code::CONSTANT_TRUE : op_code::CONSTANT_FALSE));
	  }
	  else if constexpr(std::is_same_v<T, nil_value_tag_type>)
	  {
		  emit_code(le->get_token(), V(vm::op_code::CONSTANT_NIL));
	  }
	  else if constexpr(std::is_same_v<T, string_literal_type>)
	  {
		  emit_constant(le->get_token(), string_object::create_on_heap(heap_, arg));
	  }
	  else if constexpr(!std::is_same_v<T, empty_literal_tag>) // empty literal isn't meant to be a constant
	  {
		  emit_constant(le->get_token(), arg);
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
	generate(ge->get_expr());
}

void clox::interpreting::compiling::codegen::visit_var_expression(const std::shared_ptr<var_expression>& ve)
{
	auto name = ve->get_name();

	auto binding = variable_lookup(ve);

	if (binding)
	{
		if (auto upvalue = binding->upvalue();upvalue)
		{
			emit_codes(ve->get_name(), VC(SEC_OP_UPVALUE, op_code::GET), upvalue->current_index());
		}
		else
		{
			auto symbol = binding->symbol();

			if (symbol->is_global())
			{
				emit_codes(ve->get_name(), VC(SEC_OP_GLOBAL, op_code::GET), identifier_constant(name));
			}
			else if (symbol->is_local())
			{
				emit_codes(ve->get_name(), VC(SEC_OP_LOCAL, op_code::GET), symbol->slot_index());
			}
		}
	}
	else
	{
		throw internal_codegen_error{ "Name lookup failure" };
	}

}

void clox::interpreting::compiling::codegen::visit_ternary_expression(const std::shared_ptr<ternary_expression>& te)
{
	generate(te->get_cond());

	auto false_jmp = emit_jump(te->get_colon(),
		V(op_code::JUMP_IF_FALSE)); // if cond is false, jump to false expression calculation

	emit_code(V(op_code::POP)); // pop the cond value
	generate(te->get_true_expr());  // calculate the true expression

	auto end_jmp = emit_jump(te->get_colon(), V(op_code::JUMP)); // skip the false expression

	// Here goes the false expression
	patch_jump(false_jmp);

	emit_code(V(op_code::POP)); // pop the cond value
	generate(te->get_false_expr()); // calculate the false expression

	// Here goes the end
	patch_jump(end_jmp);
}

void clox::interpreting::compiling::codegen::visit_logical_expression(const std::shared_ptr<logical_expression>& le)
{
	switch (le->get_op().type())
	{

	case scanning::token_type::AND:
	{
		// This implements a logical short circuit for logical and:
		generate(le->get_left()); // calculate lhs expression first.

		// if lhs is false, jump to end and leave the false value in the stack
		auto end_jmp = emit_jump(le->get_op(), V(op_code::JUMP_IF_FALSE));

		// if lhs is true, the value is useless so that it is discarded (poped)
		emit_code(V(op_code::POP));

		// and calculate rhs. the rhs determines the value of the expression
		generate(le->get_right());

		patch_jump(end_jmp);
		break;
	}
	case scanning::token_type::OR:
	{
		// Same goes for logical or
		generate(le->get_left()); // calculate lhs first

		// if value is false, we jump to the calculation of rhs
		auto false_jmp = emit_jump(le->get_op(), V(vm::op_code::JUMP_IF_FALSE));

		// if value is true, we reach here and jump to the end
		auto true_jmp = emit_jump(le->get_op(), V(vm::op_code::JUMP));

		// Here goes the calculation of rhs
		patch_jump(false_jmp);

		// lhs is now useless
		emit_code(V(vm::op_code::POP));

		// calculation of rhs
		generate(le->get_right());

		// Here goes the end
		patch_jump(true_jmp);

		break;
	}

	default:
		UNREACHABLE_EXCEPTION;

	}
}

void clox::interpreting::compiling::codegen::visit_call_expression(const std::shared_ptr<call_expression>& ce)
{
	if (ce->get_callee()->get_type() == parsing::PC_TYPE_base_expression)[[unlikely]]
	{
		generate(ce->get_callee());

		for (const auto& arg : ce->get_args())
		{
			generate(arg); // push arguments in the stack
		}

		emit_codes(ce->get_paren(), V(op_code::CALL), ce->get_args().size()); // call the function
	}
	else if (auto annotation = ce->get_annotation<call_annotation>();annotation &&
		annotation->is_ctor()) [[unlikely]]
	{

		emit_codes(ce->get_paren(), VC(SEC_OP_CLASS, vm::op_code::PUSH),
			identifier_constant(annotation->ctor_class_type()->name()));

		if (annotation->statement()) [[likely]]
		{
			// TODO: should it call ctor?
			emit_code(V(vm::op_code::INSTANCE));

			for (const auto& arg : ce->get_args())
			{
				generate(arg); // push arguments in the stack
			}

			emit_codes(ce->get_paren(), VC(SEC_OP_CTOR, op_code::INVOKE), annotation->id(),
				ce->get_args().size()); // invoke the method

		}
		else
		{
			emit_code(V(vm::op_code::INSTANCE));
		}
	}
	else if (annotation && !annotation->is_ctor()) [[likely]]
	{
		if (annotation->is_method())
		{
			auto get_expr = static_pointer_cast<get_expression>(ce->get_callee());
			generate(get_expr->get_object());
		}
		else
		{
			emit_codes(ce->get_paren(), VC(SEC_OP_FUNC, op_code::PUSH), annotation->id());
			emit_code(ce->get_paren(), V(op_code::CLOSURE));
		}

		for (const auto& arg : ce->get_args())
		{
			generate(arg); // push arguments in the stack
		}

		if (annotation->is_method()) [[unlikely]]
		{
			emit_codes(ce->get_paren(), V(op_code::INVOKE), annotation->id(),
				ce->get_args().size()); // invoke the method
		}
		else [[likely]]
		{
			emit_codes(ce->get_paren(), V(op_code::CALL), ce->get_args().size()); // call the function
		}

		if (annotation->is_ctor())
		{
//			emit_code(V(vm::op_code::POP)); // Pop the default nil value
		}

	}
	else [[unlikely]] // it is not a call expression that bind to certain function, so we directly deal with it
	{
		generate(ce->get_callee());

		emit_code(ce->get_paren(), V(op_code::CLOSURE));

		for (const auto& arg : ce->get_args())
		{
			generate(arg); // push arguments in the stack
		}

		emit_codes(ce->get_paren(), V(op_code::CALL), ce->get_args().size()); // call the function
	}
}

void clox::interpreting::compiling::codegen::visit_get_expression(const std::shared_ptr<get_expression>& ge)
{
	auto annotation = ge->get_annotation<class_annotation>();

	generate(ge->get_object());

	auto class_type = annotation->class_type();

	if (annotation->is_method() && !annotation->is_native())
	{
		auto caller_anno = annotation->method_caller()->get_annotation<call_annotation>();
		emit_codes(VC(SEC_OP_FUNC, vm::op_code::GET_PROPERTY), caller_anno->id());
	}
	else if (annotation->is_method() && annotation->is_native()) //native method
	{
		auto caller = annotation->method_caller();
	}
	else
	{
		if (auto field = class_type->fields().find(ge->get_name().lexeme());field != class_type->fields().end())
		{
			auto offset = distance(class_type->fields().begin(), field);

			emit_codes(ge->get_name(), V(vm::op_code::GET_PROPERTY), offset);
		}
		else
		{
			assert(false);
		}
	}
}

void clox::interpreting::compiling::codegen::visit_set_expression(const std::shared_ptr<set_expression>& se)
{
	auto annotation = se->get_annotation<class_annotation>();

	generate(se->get_object());
	generate(se->get_val());

	auto class_type = annotation->class_type();

	auto iter = class_type->fields().find(se->get_name().lexeme());

	auto offset = distance(class_type->fields().begin(), iter);

	emit_codes(se->get_name(), V(vm::op_code::SET_PROPERTY), offset);
}

void clox::interpreting::compiling::codegen::visit_expression_statement(
	const std::shared_ptr<expression_statement>& es)
{
	generate(es->get_expr());
	emit_code(V(op_code::POP));
}

void clox::interpreting::compiling::codegen::visit_print_statement(const std::shared_ptr<print_statement>& pe)
{
	generate(pe->get_expr());
	emit_code(pe->get_keyword(), V(op_code::PRINT));
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
		emit_code(vs->get_name(), V(op_code::CONSTANT_NIL));
	}

	if (auto symbol = current_scope()->find_name<named_symbol>(vs->get_name().lexeme());symbol->is_global())
	{
		define_global_variable(vs->get_name().lexeme(), identifier_constant(vs->get_name()),
			vs->get_name());
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

void clox::interpreting::compiling::codegen::visit_while_statement(const std::shared_ptr<while_statement>& ws)
{
	auto last_op =
		current_chunk()->count(); // it will be the index of first instruction for cond. Prepared for LOOP instruction

	generate(ws->get_cond());

	auto exit_jmp = emit_jump(ws->get_cond_l_paren(), V(op_code::JUMP_IF_FALSE));

	emit_code(V(vm::op_code::POP)); // pop the cond value

	generate(ws->get_body());

	emit_loop(last_op);

	patch_jump(exit_jmp);

	emit_code(V(vm::op_code::POP)); // pop the cond value, even we do not run the last loop
}

void
clox::interpreting::compiling::codegen::visit_foreach_statement(const std::shared_ptr<foreach_statement>& ptr)
{

}

void clox::interpreting::compiling::codegen::visit_if_statement(const std::shared_ptr<if_statement>& ifs)
{
	generate(ifs->get_cond());

	auto then_jmp = emit_jump(ifs->get_cond_l_paren(), V(op_code::JUMP_IF_FALSE));
	emit_code(V(op_code::POP));// pop conditional value when cond is true

	generate(ifs->get_true_stmt());

	auto else_jmp = emit_jump(ifs->get_else_keyword().value_or(ifs->get_cond_l_paren()),
		V(op_code::JUMP)); // when execute true branch, we must skip the else branch

	patch_jump(then_jmp);
	emit_code(V(op_code::POP));// pop conditional value when cond is false

	if (ifs->get_false_stmt())
	{
		generate(ifs->get_false_stmt());
	}

	patch_jump(else_jmp);

}

void
clox::interpreting::compiling::codegen::visit_function_statement(const std::shared_ptr<function_statement>& fs)
{
	auto constant = emit_constant(fs->get_name(), static_cast<function_object_raw_pointer>(nullptr));

	// define it as variable to follow the function overloading specification
	if (auto symbol = current_scope()->find_name<named_symbol>(fs->get_name().lexeme());
		symbol && symbol->is_global())
	{
		define_global_variable(fs->get_name().lexeme(), identifier_constant(fs->get_name()),
			fs->get_name());
	}
	else
	{
		// for class method, the function object is left for METHOD op, for local function, it becomes a local variable
		declare_local_variable(fs->get_name().lexeme());
	}

	auto id_ret = resolver_->function_id(fs);

	assert(id_ret.has_value());

	emit_codes(fs->get_name(), VC(SEC_OP_FUNC, vm::op_code::DEFINE), id_ret.value(), constant);

	emit_codes(VC(SEC_OP_FUNC, op_code::PUSH), id_ret.value());

	emit_code(VC(SEC_OP_CAPTURE, op_code::CLOSURE));

	scope_begin();

	emit_code(V(vm::op_code::POP));

	function_push(heap_->allocate<function_object>(fs->get_name().lexeme(), fs->get_params().size()));

	for (const auto& param : fs->get_params())
	{
		declare_local_variable(param.first.lexeme());
	}

	generate(fs->get_body());

	scope_end();

	if (auto anno = fs->get_annotation<function_annotation>();anno)
	{
		auto func = function_pop(true);
		set_constant(constant, func);
	}
	else
	{
		auto func = function_pop();
		set_constant(constant, func);
	}

}

void clox::interpreting::compiling::codegen::visit_return_statement(const std::shared_ptr<return_statement>& rs)
{
	// FIXME: return this for constructor
	generate(rs->get_val());
	emit_code(rs->get_return_keyword(), V(op_code::RETURN));
}

void clox::interpreting::compiling::codegen::visit_class_statement(const std::shared_ptr<class_statement>& class_stmt)
{
	auto name_constant = identifier_constant(class_stmt->get_name());

	auto class_type = current_scope()->type_typed<lox_class_type>(class_stmt->get_name().lexeme());

	emit_codes(class_stmt->get_name(), V(op_code::CLASS), name_constant, class_type->fields().size());
	if (class_stmt->get_base_class())
	{
		generate(class_stmt->get_base_class());
		emit_codes(class_stmt->get_name(), V(op_code::INHERIT));
	}

	define_global_variable(class_stmt->get_name().lexeme(), name_constant, class_stmt->get_name());
	emit_codes(class_stmt->get_name(), VC(SEC_OP_GLOBAL, vm::op_code::GET), name_constant);

	scope_begin();

	scope_begin();

	for (const auto& method : class_stmt->get_methods())
	{
		generate(method);
		auto id = resolver_->function_id(method);

		//FIXME
		assert(id.has_value());

		emit_codes(method->get_name(), V(vm::op_code::METHOD), id.value());

	}

	scope_end();
	scope_end();

	emit_code(V(op_code::POP)); // pop the class object
}

std::shared_ptr<vm::chunk> codegen::current_chunk()
{
	return functions_.back()->body();
}

void codegen::emit_code(vm::full_opcode_type byte)
{
	current_chunk()->write(byte);
}

void codegen::emit_code(const token& lead_token, vm::full_opcode_type byte)
{
	current_chunk()->write(byte, lead_token);
}

void codegen::emit_return()
{
	emit_code(V(op_code::CONSTANT_NIL));
	emit_code(V(op_code::RETURN));
}

vm::chunk::code_type codegen::emit_constant(const scanning::token& tk, const value& val)
{
	auto constant = make_constant(val);
	emit_codes(tk, V(op_code::CONSTANT), constant);
	return constant;
}

uint16_t codegen::make_constant(const value& val)
{
	auto idx = current_chunk()->add_constant(val);
	return idx;
}

void codegen::set_constant(vm::full_opcode_type pos, const value& val)
{
	current_chunk()->constant_at(pos) = val;
}

void codegen::define_global_variable(const std::string& name, vm::chunk::code_type global,
	std::optional<scanning::token> tk)
{
//	local_scopes_.front()->declare(name, local_scope::GLOBAL_SLOT);
	if (tk.has_value())[[likely]]
	{
		emit_codes(tk.value(), VC(SEC_OP_GLOBAL, op_code::DEFINE), global);
	}
	else
	{
		emit_codes(VC(SEC_OP_GLOBAL, op_code::DEFINE), global);
	}
}

void codegen::declare_local_variable(const string& name, size_t depth)
{
//	(*(local_scopes_.rbegin() + depth))->declare(name, local_totals_);
//	local_totals_++;
}

uint16_t codegen::identifier_constant(const token& identifier)
{
	return identifier_constant(identifier.lexeme());
}

uint16_t codegen::identifier_constant(const string& lexeme)
{
	return make_constant(lexeme);
}

shared_ptr<named_symbol> codegen::variable_lookup(const string& name)
{
	return current_scope()->find_name<named_symbol>(name);
}

std::shared_ptr<resolving::variable_annotation> codegen::variable_lookup(const shared_ptr<expression>& expr)
{
	return expr->get_annotation<variable_annotation>();
}

vm::chunk::difference_type codegen::emit_jump(const token& lead_token, vm::full_opcode_type jmp)
{
	emit_code(jmp);
	emit_code(PATCHABLE_PLACEHOLDER);

	return current_chunk()->count() - 1;
}

void codegen::patch_jump(vm::chunk::difference_type pos)
{
	auto dist = current_chunk()->count() - 1 - pos;
	if (dist > numeric_limits<full_opcode_type>::max())
	{
		throw jump_too_long{ static_cast<size_t>(dist) };
	}

	current_chunk()->patch_begin(dist, pos);
}

void codegen::emit_loop(vm::chunk::difference_type pos)
{
	auto dist = current_chunk()->count() - pos + 2;
	if (dist > numeric_limits<full_opcode_type>::max())
	{
		throw jump_too_long{ static_cast<size_t>(dist) };
	}

	emit_code(V(op_code::LOOP));
	emit_code(dist);
}

void codegen::function_push(vm::function_object_raw_pointer func)
{
	functions_.push_back(func);
}

vm::function_object_raw_pointer codegen::function_pop(bool is_in_ctor)
{
	//TODO: print disassembly when errors occur
	if (is_in_ctor)
	{
		emit_codes(VC(SEC_OP_LOCAL, op_code::GET), 0);
		emit_code(V(op_code::RETURN));
	}
	else
	{
		emit_return();
	}

	auto top = function_top();
	functions_.pop_back();

	return top;
}

vm::function_object_raw_pointer codegen::function_top()
{
	return functions_.back();
}

vm::closure_object_raw_pointer codegen::top_level()
{
	return heap_->allocate<closure_object>(function_top());
}

void codegen::visit_lambda_expression(const std::shared_ptr<lambda_expression>& ptr)
{

}

void codegen::visit_list_initializer_expression(const std::shared_ptr<list_initializer_expression>& lie)
{
	for (const auto& val : lie->get_values())
	{
		generate(val);
	}

	emit_codes(V(vm::op_code::MAKE_LIST), lie->get_values().size());
}

void codegen::visit_map_initializer_expression(const std::shared_ptr<struct map_initializer_expression>& ptr)
{

}


