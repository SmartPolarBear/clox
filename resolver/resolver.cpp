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
// Created by cleve on 7/1/2021.
//

#include <resolver/resolver.h>
#include <resolver/lox_type.h>

#include <logger/logger.h>

#include <memory>
#include <format>
#include <ranges>
#include <tuple>

#include <gsl/gsl>

using namespace clox::parsing;
using namespace clox::logging;
using namespace clox::resolving;
using namespace clox::interpreting;

using namespace std;
using namespace gsl;

resolver::resolver(shared_ptr<symbol_table> st) :
		symbols_(std::move(st)),
		global_scope_{ std::make_shared<scope>() }
{
	global_scope_->types()["object"] = lox_object_type::object();

	global_scope_->types()["integer"] = lox_object_type::integer();
	global_scope_->types()["floating"] = lox_object_type::floating();
	global_scope_->types()["boolean"] = lox_object_type::boolean();
	global_scope_->types()["nil"] = lox_object_type::nil();
}


std::shared_ptr<lox_type>
resolver::visit_assignment_expression(const std::shared_ptr<parsing::assignment_expression>& e)
{
	auto value_type = resolve(e->get_value());

	// set the depth of expression
	resolve_local(e, e->get_name());

	auto target_type = scope_top()->type_of_names()[e->get_name().lexeme()];

	auto compa = check_type_assignment(e->get_name(), target_type, value_type);
	// TODO

	return get<0>(compa);
}

std::shared_ptr<lox_type> resolver::visit_binary_expression(const std::shared_ptr<parsing::binary_expression>& expr)
{
	auto l_type = resolve(expr->get_left());
	auto r_type = resolve(expr->get_right());

	auto ret = check_type_binary_expression(expr->get_op(), l_type, r_type);

	return get<0>(ret);
}

std::shared_ptr<lox_type> resolver::visit_unary_expression(const std::shared_ptr<parsing::unary_expression>& ue)
{
	auto type = resolve(ue->get_right());

	auto ret = check_type_unary_expression(ue->get_op(), type);
	return get<0>(ret);
}

std::shared_ptr<lox_type> resolver::visit_postfix_expression(const std::shared_ptr<parsing::postfix_expression>& pe)
{
	auto type = resolve(pe->get_left());
	auto ret = check_type_postfix_expression(pe->get_op(), type);

	return get<0>(ret);
}


std::shared_ptr<lox_type> resolver::visit_literal_expression(const std::shared_ptr<parsing::literal_expression>& le)
{
	if (holds_alternative<long long>(le->get_value()))
	{
		return lox_object_type::integer();
	}
	else if (holds_alternative<long double>(le->get_value()))
	{
		return lox_object_type::floating();
	}
	else if (holds_alternative<bool>(le->get_value()))
	{
		return lox_object_type::boolean();
	}
	else if (holds_alternative<scanning::nil_value_tag_type>(le->get_value()))
	{
		return lox_object_type::nil();
	}
	else if (holds_alternative<std::string>(le->get_value()))
	{
		// TODO
		return lox_object_type::object();
	}
	else
	{
		// TODO
		return lox_object_type::object();
	}
}

std::shared_ptr<lox_type> resolver::visit_grouping_expression(const std::shared_ptr<parsing::grouping_expression>& ge)
{
	return resolve(ge->get_expr());
}

std::shared_ptr<lox_type> resolver::visit_var_expression(const std::shared_ptr<parsing::var_expression>& ve)
{
	if (!scopes_.empty())
	{
		if (auto existence = scope_top_find(ve->get_name().lexeme());existence.has_value() &&
																	 !existence.value())
		{
			logger::instance().error(ve->get_name(),
					std::format("Reading {} before its initialization.", ve->get_name().lexeme()));
		}
	}

	resolve_local(ve, ve->get_name());

	return scope_top()->type_of_names()[ve->get_name().lexeme()];
}

std::shared_ptr<lox_type> resolver::visit_ternary_expression(const std::shared_ptr<parsing::ternary_expression>& te)
{
	auto cond_type = resolve(te->get_cond());
	auto t_type = resolve(te->get_true_expr());
	auto f_type = resolve(te->get_false_expr());

//	if (check_type_implicit_convertible(te->get_qmark(), cond_type, make_shared<boolean_type>()))
//	{
//
//	}

	auto value_type_ret = check_type_ternary_expression(te->get_colon(), t_type, f_type);

	return get<0>(value_type_ret);
}

std::shared_ptr<lox_type> resolver::visit_logical_expression(const std::shared_ptr<parsing::logical_expression>& le)
{
	auto l_type = resolve(le->get_left());
	auto r_type = resolve(le->get_right());

	auto ret = check_type_logical_expression(le->get_op(), l_type, r_type);

	return get<0>(ret);
}


std::shared_ptr<lox_type> resolver::visit_get_expression(const std::shared_ptr<get_expression>& ptr)
{
	return resolve(ptr->get_object());
}

std::shared_ptr<lox_type> resolver::visit_set_expression(const std::shared_ptr<set_expression>& se)
{
	auto target_type = resolve(se->get_val());
	auto value_type = resolve(se->get_object());

	auto ret = check_type_assignment(se->get_name(), target_type, value_type);

	return get<0>(ret);
}

std::shared_ptr<lox_type> resolver::visit_this_expression(const std::shared_ptr<this_expression>& expr)
{
	if (cur_cls_ == class_type::CT_NONE)
	{
		logger::instance().error(expr->get_keyword(), "Can't use this in standalone function or in global scoop.");
		// TODO
		return nullptr;
	}

	resolve_local(expr, expr->get_keyword());

	// TODO
	return nullptr;
}

std::shared_ptr<lox_type> resolver::visit_base_expression(const std::shared_ptr<base_expression>& be)
{
	if (cur_cls_ == class_type::CT_NONE)
	{
		logger::instance().error(be->get_keyword(), "Can't use super in standalone function or in global scoop.");
		// TODO
		return nullptr;
	}
	else if (cur_cls_ != class_type::CT_INHERITED_CLASS)
	{
		logger::instance().error(be->get_keyword(), "Can't use super in class who doesn't have a base class.");
		// TODO
		return nullptr;
	}

	resolve_local(be, be->get_keyword());
	// TODO
	return nullptr;
}


std::shared_ptr<lox_type> resolver::visit_call_expression(const std::shared_ptr<parsing::call_expression>& ce)
{
	resolve(ce->get_calle());

	for (const auto& arg:ce->get_args())
	{
		resolve(arg);
	}

	// TODO
	return nullptr;
}

void resolver::visit_expression_statement(const std::shared_ptr<parsing::expression_statement>& es)
{
	resolve(es->get_expr());
}

void resolver::visit_print_statement(const std::shared_ptr<parsing::print_statement>& pe)
{
	resolve(pe->get_expr());
}

void resolver::visit_variable_statement(const std::shared_ptr<parsing::variable_statement>& stmt)
{
	declare_name(stmt->get_name());

	shared_ptr<lox_type> initializer_type{ nullptr };
	if (stmt->get_initializer())
	{
		resolve(stmt->get_initializer());
	}

	shared_ptr<lox_type> declared_type{ nullptr };
	if (stmt->get_type_expr())
	{
		declared_type = resolve(stmt->get_type_expr());
	}

	if (!initializer_type && !declared_type)
	{
		logger::instance().error(stmt->get_name(),
				"Variable declaration should either has a given type, or have a type-deducible initializer expression, or both.");
	}

	if (initializer_type && declared_type)
	{
		auto compatibility = check_type_assignment(stmt->get_name(), declared_type, initializer_type);
		if (!get<1>(compatibility)) // narrowing conversion
		{
		}
	}

	this->define_name(stmt->get_name(), declared_type ? declared_type : initializer_type);
}

void resolver::visit_block_statement(const std::shared_ptr<parsing::block_statement>& blk)
{
	scope_begin();
	auto _ = finally([this]
	{
		this->scope_end();
	});

	resolve(blk->get_stmts());
}

void resolver::visit_while_statement(const std::shared_ptr<parsing::while_statement>& ws)
{
	resolve(ws->get_cond());
	resolve(ws->get_body());
}

void resolver::visit_if_statement(const std::shared_ptr<parsing::if_statement>& stmt)
{
	resolve(stmt->get_cond());
	resolve(stmt->get_true_stmt());
	if (stmt->get_false_stmt())resolve(stmt->get_false_stmt());
}

void resolver::visit_function_statement(const std::shared_ptr<parsing::function_statement>& stmt)
{
	declare_name(stmt->get_name());
	define_name(stmt->get_name(), nullptr/*FIXME*/);

	resolve_function(stmt, function_type::FT_FUNCTION);
}

void resolver::visit_return_statement(const std::shared_ptr<parsing::return_statement>& rs)
{
	if (cur_func_ == function_type::FT_NONE)
	{
		logger::instance().error(rs->get_return_keyword(), "Return statement in none-function scoop.");
	}

	if (rs->get_val())
	{
		if (cur_func_ == function_type::FT_CTOR)
		{
			logger::instance().error(rs->get_return_keyword(), "Constructor can't return a value.");
		}

		resolve(rs->get_val());
	}
}

void resolver::resolve(const std::vector<std::shared_ptr<parsing::statement>>& stmts)
{
	for (const auto& stmt:stmts)
	{
		resolve(stmt);
	}
}

void resolver::resolve(const std::shared_ptr<parsing::statement>& stmt)
{
	accept(*stmt, *this);
}

std::shared_ptr<lox_type> resolver::resolve(const std::shared_ptr<clox::parsing::expression>& expr)
{
	return accept(*expr, *this);
}

std::shared_ptr<lox_type> resolver::resolve(const shared_ptr<parsing::type_expression>& expr)
{
	return accept(*expr, *this);
}


void resolver::scope_begin()
{
	scope_push(make_shared<scope>());
}

void resolver::scope_end()
{
	scope_pop();
}

void resolver::declare_name(const clox::scanning::token& t)
{
	if (scopes_.empty())return;

	auto top = scope_top();

	if (top->names().contains(t.lexeme()))
	{
		logger::instance().error(t, std::format("{} already exists in this scoop.", t.lexeme()));
	}

	top->names()[t.lexeme()] = false;
}

void resolver::define_name(const clox::scanning::token& tk, const std::shared_ptr<lox_type>& type)
{
	if (scopes_.empty())return;

	scope_top()->names()[tk.lexeme()] = true;
	scope_top()->type_of_names()[tk.lexeme()] = type;
}

void resolver::define_type(const clox::scanning::token& tk, const lox_type& type, uint64_t depth)
{
//	if (scopes_.empty())return;
//	scopes_[depth]->types()[tk.lexeme()] = type;
}


void resolver::resolve_local(const shared_ptr<parsing::expression>& expr, const clox::scanning::token& tk)
{
	int64_t depth = 0;

	for (const auto& s:scopes_ | views::reverse) // traverse from the stack top, which has a depth of zero.
	{
		if (s->names().contains(tk.lexeme()))
		{

			symbols_->set_depth(expr, depth);

			return;
		}
		depth++;
	}
}

void resolver::resolve_function(const shared_ptr<parsing::function_statement>& func, function_type type)
{
	auto parent_type = cur_func_;
	cur_func_ = type;
	scope_begin();

	auto _ = finally([this, parent_type]
	{
		this->scope_end();
		cur_func_ = parent_type;
	});

	for (const auto& tk:func->get_params())
	{
		declare_name(tk);
		define_name(tk, nullptr/*FIXME*/);
	}

	resolve(func->get_body());
}

std::shared_ptr<lox_type> resolver::type_lookup(const scanning::token& tk)
{
	for (auto& scoop:scopes_)
	{
		if (scoop->types().contains(tk.lexeme()))
		{
			return scoop->types().at(tk.lexeme());
		}
	}

	return type_error(tk, std::format("Type {} is not defined.", tk.lexeme()));
}


void resolver::visit_class_statement(const std::shared_ptr<class_statement>& cls)
{
	class_type enclosing = cur_cls_;
	cur_cls_ = class_type::CT_CLASS;

	declare_name(cls->get_name());
	define_name(cls->get_name(), nullptr/*FIXME*/);

	if (cls->get_base_class() && cls->get_base_class()->get_name().lexeme() == cls->get_name().lexeme())
	{
		logger::instance().error(cls->get_base_class()->get_name(), "A class cannot inherit from itself.");
	}

	if (cls->get_base_class())
	{
		cur_cls_ = class_type::CT_INHERITED_CLASS;
		resolve(cls->get_base_class());
	}

	if (cls->get_base_class())
	{
		scope_begin();
		scope_top()->names()["base"] = true;
	}

	scope_begin();
	auto _ = finally([this, &cls, enclosing]
	{
		this->scope_end();

		if (cls->get_base_class())
		{
			this->scope_end();
		}

		this->cur_cls_ = enclosing;
	});

	scope_top()->names()["this"] = true;

	for (const auto& method:cls->get_methods())
	{
		auto decl = function_type::FT_METHOD;
		if (method->get_name().lexeme() == "init")
		{
			decl = function_type::FT_CTOR;
		}
		resolve_function(method, decl);
	}


}


std::shared_ptr<lox_type> resolver::visit_variable_type_expression(const std::shared_ptr<variable_type_expression>& vte)
{
	return type_lookup(vte->get_name());
}


std::shared_ptr<lox_type> resolver::type_error(const clox::scanning::token& tk, const std::string& msg)
{
	logger::instance().error(tk, msg);
	return make_shared<lox_any_type>();
}

type_compatibility
resolver::check_type_assignment(const clox::scanning::token& tk, const shared_ptr<lox_type>& left,
		const shared_ptr<lox_type>& right)
{
	if (lox_type::is_primitive(*left) && lox_type::is_primitive(*right))
	{
		auto compatible = lox_type::unify(*left, *right);

		auto type = compatible ? left : nullptr;

		return make_tuple(
				type,
				compatible,
				false
		);
	}

	return make_tuple(type_error(tk, std::format(R"({} of type "{}" is not assignable for type "{}")",
					tk.lexeme(),
					scope_top()->type_of_names()[tk.lexeme()]->printable_string(),
					right->printable_string())),
			false,
			false);
}

type_compatibility
resolver::check_type_binary_expression(const clox::scanning::token& tk, const shared_ptr<lox_type>& left,
		const shared_ptr<lox_type>& right)
{
	return clox::resolving::type_compatibility();
}

type_compatibility
resolver::check_type_unary_expression(const clox::scanning::token& tk, const shared_ptr<lox_type>& left)
{
	return clox::resolving::type_compatibility();
}

type_compatibility
resolver::check_type_postfix_expression(const clox::scanning::token& tk, const shared_ptr<lox_type>& right)
{
	return clox::resolving::type_compatibility();
}

bool resolver::check_type_implicit_convertible(const clox::scanning::token& tk, const shared_ptr<lox_type>& left,
		const shared_ptr<lox_type>& right)
{
	return false;
}

type_compatibility
resolver::check_type_ternary_expression(const clox::scanning::token& tk, const shared_ptr<lox_type>& left,
		const shared_ptr<lox_type>& right)
{
	return clox::resolving::type_compatibility();
}

type_compatibility
resolver::check_type_logical_expression(const clox::scanning::token& tk, const shared_ptr<lox_type>& left,
		const shared_ptr<lox_type>& right)
{
	return clox::resolving::type_compatibility();
}
