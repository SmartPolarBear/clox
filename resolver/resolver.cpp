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
#include <resolver/object_type.h>
#include <resolver/callable_type.h>
#include <resolver/instance_type.h>

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

	global_scope_->types()["string"] = lox_object_type::string();

	cur_func_.push(env_function_type::FT_NONE);
	cur_class_.push(env_class_type::CT_NONE);
}


std::shared_ptr<lox_type>
resolver::visit_assignment_expression(const std::shared_ptr<parsing::assignment_expression>& e)
{
	auto value_type = resolve(e->get_value());

	// set the depth of expression
	auto symbol = resolve_local(e, e->get_name());

	auto compa = check_type_assignment(e->get_name(), symbol->type(), value_type);

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
		return lox_object_type::string();
	}
	else
	{
		return type_error(le->get_token(), "Invalid literal value of unknown type.");
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

	auto symbol = resolve_local(ve, ve->get_name());

	return symbol->type();
}

std::shared_ptr<lox_type> resolver::visit_ternary_expression(const std::shared_ptr<parsing::ternary_expression>& te)
{
	auto cond_type = resolve(te->get_cond());
	auto t_type = resolve(te->get_true_expr());
	auto f_type = resolve(te->get_false_expr());

	if (!lox_type::unify(*lox_object_type::boolean(), *cond_type))
	{
		return type_error(te->get_qmark(), "The condition for ternary operator should be compatible with boolean.");
	}

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
	auto obj_type = resolve(ptr->get_object());

	if (!lox_type::is_instance(*obj_type))
	{
		return type_error(ptr->get_name(), std::format("{} is not a instance", ptr->get_name().lexeme()));
	}

	auto inst = static_pointer_cast<lox_instance_type>(obj_type);

	if (!lox_type::is_class(*inst->underlying_type()))
	{
		return type_error(ptr->get_name(), std::format("{} of type {} is not a class type", ptr->get_name().lexeme(),
				inst->underlying_type()->printable_string()));
	}

	auto class_type = static_pointer_cast<lox_class_type>(inst->underlying_type());
	auto member_name = ptr->get_name().lexeme();

	if (class_type->fields().contains(member_name))
	{
		return class_type->fields().at(member_name);
	}
	else if (class_type->methods().contains(member_name))
	{
		return class_type->methods().at(member_name);
	}

	return type_error(ptr->get_name(), std::format("Instance of type {} do not have a member named {}",
			class_type->printable_string(), member_name));
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

	if (cur_class_.top() == env_class_type::CT_NONE)
	{
		logger::instance().error(expr->get_keyword(), "Can't use this in standalone function or in global scoop.");
		return make_shared<lox_any_type>();
	}

	auto symbol = resolve_local(expr, expr->get_keyword());

	return symbol->type();
}

std::shared_ptr<lox_type> resolver::visit_base_expression(const std::shared_ptr<base_expression>& be)
{
	if (cur_class_.top() == env_class_type::CT_NONE)
	{
		logger::instance().error(be->get_keyword(), "Can't use super in standalone function or in global scoop.");
		return make_shared<lox_any_type>();
	}
	else if (cur_class_.top() != env_class_type::CT_INHERITED_CLASS)
	{
		logger::instance().error(be->get_keyword(), "Can't use super in class who doesn't have a base class.");
		return make_shared<lox_any_type>();
	}

	auto symbol = resolve_local(be, be->get_keyword());

	return symbol->type();
}


std::shared_ptr<lox_type> resolver::visit_call_expression(const std::shared_ptr<parsing::call_expression>& ce)
{
	auto callee = resolve(ce->get_callee());

	if (!callee)
	{
		throw logic_error{ "callee isn't nullable" };
	}

	shared_ptr<lox_callable_type> callable{ nullptr };

	if (lox_type::is_instance(*callee))
	{
		auto underlying = static_pointer_cast<lox_instance_type>(callee)
				->underlying_type();

		if (lox_type::is_callable(*underlying))
		{
			callable = static_pointer_cast<lox_callable_type>(underlying);
		}
	}
	else if (lox_type::is_callable(*callee))
	{
		callable = static_pointer_cast<lox_callable_type>(callee);
	}
	else if (lox_type::is_class(*callee))
	{
		auto class_t = static_pointer_cast<lox_class_type>(callee);
		callable = static_pointer_cast<lox_callable_type>(class_t->methods().at("init"));
	}

	if (!callable)
	{
		return type_error(ce->get_paren(),
				std::format("Type {} is neither a callable nor a class", callee->printable_string()));
	}

	if (ce->get_args().size() != callable->param_size())
	{
		return type_error(ce->get_paren(),
				std::format("{} args are needed, but {} are given", callable->param_size(), ce->get_args().size()));
	}

	auto size = callable->param_size();
	for (decltype(size) i = 0; i < size; i++)
	{
		auto arg_type = resolve(ce->get_args()[i]);
		auto param_type = callable->param_type(i);
		if (!lox_type::unify(*param_type, *arg_type))
		{
			return type_error(ce->get_paren(),
					std::format("{}th argument of type {} is not compatible with parameter with type {}.",
							i, arg_type->printable_string(), param_type->printable_string()));
		}
	}

	if (lox_type::is_callable(*callee) || lox_type::is_instance(*callee))
	{
		return callable->return_type();
	}
	else if (lox_type::is_class(*callee))
	{
		return static_pointer_cast<lox_class_type>(callee);
	}

	throw logic_error{ "should not reach here" };
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
		initializer_type = resolve(stmt->get_initializer());
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
		return;
	}

	if (initializer_type && declared_type)
	{
		auto compatibility = check_type_assignment(stmt->get_name(), declared_type, initializer_type);
		if (!get<1>(compatibility)) // narrowing conversion
		{
			// FIXME
		}
	}

	auto var_type = declared_type ? declared_type : initializer_type;

	if (lox_type::is_class(*var_type))
	{
		var_type = make_shared<lox_instance_type>(static_pointer_cast<lox_class_type>(var_type));
	}

	this->define_name(stmt->get_name(), var_type);
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
	auto cond_type = resolve(ws->get_cond());
	if (!lox_type::unify(*lox_object_type::boolean(), *cond_type))
	{
		type_error(ws->get_cond_l_paren(), std::format("Condition expression of while of type {} is not subtype of {}",
				cond_type->printable_string(),
				lox_object_type::boolean()->printable_string()));
	}

	resolve(ws->get_body());
}

void resolver::visit_if_statement(const std::shared_ptr<parsing::if_statement>& stmt)
{
	auto cond_type = resolve(stmt->get_cond());
	if (!lox_type::unify(*lox_object_type::boolean(), *cond_type))
	{
		type_error(stmt->get_cond_l_paren(),
				std::format("Condition expression of if statement of type {} is not subtype of {}",
						cond_type->printable_string(),
						lox_object_type::boolean()->printable_string()));
	}

	resolve(stmt->get_true_stmt());

	if (stmt->get_false_stmt())
	{
		resolve(stmt->get_false_stmt());
	}
}

void resolver::visit_function_statement(const std::shared_ptr<parsing::function_statement>& stmt)
{
	declare_name(stmt->get_name());

	auto func_type = resolve_function_decl(stmt);

	define_name(stmt->get_name(), func_type);

	cur_func_type_.push(func_type);

	auto _ = gsl::finally([this]
	{
		cur_func_type_.pop();
	});

	resolve_function_body(stmt, env_function_type::FT_FUNCTION);
}

void resolver::visit_return_statement(const std::shared_ptr<parsing::return_statement>& rs)
{

	if (cur_func_.top() == env_function_type::FT_NONE)
	{
		logger::instance().error(rs->get_return_keyword(), "Return statement in none-function scoop.");
		return;
	}

	auto func = cur_func_type_.top();

	if (rs->get_val())
	{
		if (cur_func_.top() == env_function_type::FT_CTOR)
		{
			logger::instance().error(rs->get_return_keyword(), "Constructor can't return a value.");
		}

		auto type = resolve(rs->get_val());

		if (!func->return_type_deduced())
		{
			func->set_return_type(type);
		}
		else if (!lox_type::unify(*func->return_type(), *type))
		{
			type_error(rs->get_return_keyword(),
					std::format("Incompatible type for return expression of type {} and the function return type of {}",
							type->printable_string(),
							func->return_type()->printable_string()));
		}
	}
	else if (!func->return_type_deduced())
	{
		func->set_return_type(make_shared<lox_void_type>());
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
	if (!expr)
	{
		throw invalid_argument{ "expr" };
	}
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

void resolver::declare_name(const clox::scanning::token& t, size_t dist)
{
	if (scopes_.empty())return;

	auto top = scope_top(dist);

	if (top->names().contains(t.lexeme()))
	{
		logger::instance().error(t, std::format("{} already exists in this scoop.", t.lexeme()));
	}

	top->names()[t.lexeme()] = false;
}

void resolver::define_name(const clox::scanning::token& tk, const std::shared_ptr<lox_type>& type, size_t dist)
{
	define_name(tk.lexeme(), type, dist);
}

void resolver::define_name(const string& tk, const shared_ptr<lox_type>& type, size_t dist)
{
	if (scopes_.empty())return;

	scope_top(dist)->names()[tk] = true;
	scope_top(dist)->type_of_names()[tk] = type;
}


void resolver::define_type(const clox::scanning::token& tk, const shared_ptr<lox_type>& type, uint64_t dist)
{
	if (lox_type::is_instance(*type))
	{
		throw invalid_argument{ "type" };
	}

	if (scopes_.empty())return;
	scope_top(dist)->types()[tk.lexeme()] = type;
}

std::shared_ptr<symbol> resolver::resolve_local(const shared_ptr<expression>& expr, const clox::scanning::token& tk)
{
	int64_t depth = 0;

	for (const auto& s:scopes_ | views::reverse) // traverse from the stack top, which has a depth of zero.
	{
		if (s->names().contains(tk.lexeme()))
		{

			symbols_->set_depth(expr, depth);
			symbols_->set_type(expr, s->type_of_names().at(tk.lexeme()));

			return symbols_->at(expr);
		}
		depth++;
	}

	return nullptr;
}

shared_ptr<lox_callable_type> resolver::resolve_function_decl(const shared_ptr<function_statement>& func)
{
	lox_callable_type::return_type_variant ret_{ type_deduce_defer };
	if (func->get_return_type_expr())
	{
		ret_ = resolve(func->get_return_type_expr());
	}

	vector<pair<scanning::token, shared_ptr<lox_type>>> params_{};
	for (const auto& param:func->get_params())
	{
		shared_ptr<lox_type> param_type{ resolve(param.second) };
		params_.emplace_back(param.first, param_type);
	}

	bool ctor = false;
	if (cur_class_.top() != env_class_type::CT_NONE)
	{
		if (func->get_name().lexeme() == "init")
		{
			ctor = true;
		}
	}

	auto callable_type = make_shared<lox_callable_type>(func->get_name().lexeme(),
			ret_,
			params_,
			ctor);

	return callable_type;
}

void resolver::resolve_function_body(const shared_ptr<parsing::function_statement>& func, env_function_type type)
{
	auto func_type = cur_func_type_.top();

	cur_func_.push(type);
	scope_begin();

	auto _ = finally([this]
	{
		this->scope_end();
		cur_func_.pop();
	});

	for (const auto& param:func_type->params())
	{
		declare_name(param.first);
		define_name(param.first, param.second);
	}

	resolve(func->get_body()); // resolve the body
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
	cur_class_.push(env_class_type::CT_CLASS);

	auto[class_type, base_class_type, this_type, base_type] = resolve_class_type_decl(cls);

	/* The scope structure for class:
	 * scope {
	 * 	base:base_type
	 * 	scope {
	 * 	 this:this_type
	 * 	 (other fields)
	 * 	 (other methods)
	 * 	}
	 * }
	 * */

	declare_name(cls->get_name());
	define_name(cls->get_name(), class_type); //FIXME
	define_type(cls->get_name(), class_type);

	scope_begin();
	define_name("base", base_type);

	scope_begin();
	define_name("this", this_type);

	auto _ = finally([this]
	{
		this->scope_end();
		this->scope_end();

		this->cur_class_.pop();
	});

	resolve_class_members(cls);
}


std::tuple<shared_ptr<lox_class_type>,
		std::shared_ptr<lox_class_type>,
		shared_ptr<lox_instance_type>,
		shared_ptr<lox_instance_type>>
resolver::resolve_class_type_decl(const shared_ptr<class_statement>& cls)
{
	if (cls->get_base_class() && cls->get_base_class()->get_name().lexeme() == cls->get_name().lexeme())
	{
		logger::instance().error(cls->get_base_class()->get_name(), "A class cannot inherit from itself.");
	}

	auto base_type = static_pointer_cast<lox_type>(lox_object_type::object());
	if (cls->get_base_class())
	{
		cur_class_.top() = env_class_type::CT_INHERITED_CLASS; // it's an inherited class
		auto base = resolve(cls->get_base_class());
		if (!lox_type::is_class(*base))
		{
			base = type_error(cls->get_base_class()->get_name(),
					std::format("{} cannot be resolved to be a class type",
							cls->get_base_class()->get_name().lexeme()));
		}

		base_type = base;
	}

	auto this_type = make_shared<lox_class_type>(cls->get_name().lexeme(),
			static_pointer_cast<lox_object_type>(base_type));

	for (const auto& field:cls->get_fields())
	{
		this_type->fields()[field->get_name().lexeme()] = resolve(field->get_type_expr()); //FIXME
	}

	for (const auto& method:cls->get_methods())
	{
		this_type->methods()[method->get_name().lexeme()] = resolve_function_decl(method);
	}

	return { static_pointer_cast<lox_class_type>(this_type),
			 static_pointer_cast<lox_class_type>(base_type),
			 make_shared<lox_instance_type>(static_pointer_cast<lox_class_type>(this_type)),
			 make_shared<lox_instance_type>(static_pointer_cast<lox_class_type>(base_type)) };
}


void resolver::resolve_class_members(const shared_ptr<parsing::class_statement>& cls)
{
	for (const auto& field:cls->get_fields())
	{
		resolve(field);
	}

	for (const auto& method:cls->get_methods())
	{
		auto decl = env_function_type::FT_METHOD;
		if (method->get_name().lexeme() == "init")
		{
			decl = env_function_type::FT_CTOR;
		}

		auto func_type = resolve_function_decl(method);

		cur_func_type_.push(func_type);

		auto _ = gsl::finally([this]
		{
			cur_func_type_.pop();
		});

		resolve_function_body(method, decl);
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
resolver::check_type_assignment(const clox::scanning::token& tk, const shared_ptr<lox_type>& l,
		const shared_ptr<lox_type>& r)
{
	shared_ptr<lox_type> left{ l }, right{ r };

	if (lox_type::is_instance(*left))left = static_pointer_cast<lox_instance_type>(left)->underlying_type();
	if (lox_type::is_instance(*right))right = static_pointer_cast<lox_instance_type>(right)->underlying_type();

	auto compatible = lox_type::unify(*left, *right);
	if (compatible)
	{
		auto narrowing = !lox_type::unify(*left, *right) && lox_type::unify(*right, *left);

		auto type = compatible ? left : type_error(tk, std::format(R"({} of type "{}" is not assignable for type "{}")",
				tk.lexeme(),
				scope_top()->type_of_names().at(tk.lexeme())->printable_string(),
				right->printable_string()));


		return make_tuple(
				type,
				compatible,
				narrowing
		);
	}

	return make_tuple(type_error(tk, std::format(R"({} of type "{}" is not assignable for type "{}")",
					tk.lexeme(),
					scope_top()->type_of_names().at(tk.lexeme())->printable_string(),
					right->printable_string())),
			false,
			false);
}

type_compatibility
resolver::check_type_binary_expression(const clox::scanning::token& tk, const shared_ptr<lox_type>& l,
		const shared_ptr<lox_type>& r)
{
	shared_ptr<lox_type> left{ l }, right{ r };

	if (lox_type::is_instance(*left))left = static_pointer_cast<lox_instance_type>(left)->underlying_type();
	if (lox_type::is_instance(*right))right = static_pointer_cast<lox_instance_type>(right)->underlying_type();

	switch (tk.type())
	{
	case scanning::token_type::PLUS:
	case scanning::token_type::MINUS:
	case scanning::token_type::STAR:
	case scanning::token_type::SLASH:
		if (lox_type::is_primitive(*left) && lox_type::is_primitive(*right))
		{
			auto possible_types = { lox_object_type::boolean(), lox_object_type::integer(),
									lox_object_type::floating() };

			for (const auto& t:possible_types) // not  call intersect for extensibility
			{
				if (lox_type::unify(*t, *left) && lox_type::unify(*t, *right))
				{
					return make_tuple(t, true, false);
				}
			}
		}
		break;

	case scanning::token_type::LESS:
	case scanning::token_type::GREATER:
	case scanning::token_type::LESS_EQUAL:
	case scanning::token_type::GREATER_EQUAL:
		if (lox_type::is_primitive(*left) && lox_type::is_primitive(*right))
		{
			auto possible_types = { lox_object_type::boolean(), lox_object_type::integer(),
									lox_object_type::floating() };

			for (const auto& t:possible_types) // not  call intersect for extensibility
			{
				if (lox_type::unify(*t, *left) && lox_type::unify(*t, *right))
				{
					return make_tuple(lox_object_type::boolean(), true, false);
				}
			}
		}

	case scanning::token_type::EQUAL_EQUAL:
		// TODO: any checking?
		return make_tuple(lox_object_type::boolean(), true, false);

	case scanning::token_type::COMMA:
		return make_tuple(right, true, false);
	default:
		break;
	}

	return make_tuple(type_error(tk, std::format(R"( cannot do operator {} for type {} and {} )",
					tk.lexeme(),
					left->printable_string(),
					right->printable_string())),
			false,
			false);
}

type_compatibility
resolver::check_type_unary_expression(const clox::scanning::token& tk, const shared_ptr<lox_type>& l)
{
	shared_ptr<lox_type> left{ l };

	if (lox_type::is_instance(*left))left = static_pointer_cast<lox_instance_type>(left)->underlying_type();

	switch (tk.type())
	{
	case scanning::token_type::BANG:
		return make_tuple(lox_object_type::boolean(), true, false); // everything can be used with operator!
	case scanning::token_type::PLUS_PLUS:
	case scanning::token_type::MINUS_MINUS:
		if (lox_type::is_primitive(*left))
		{
			auto possible_types = { lox_object_type::integer(), lox_object_type::floating() };

			for (const auto& t:possible_types)
			{
				if (lox_type::unify(*t, *left))
				{
					return make_tuple(t, true, false);
				}
			}
		}
	}

	return make_tuple(type_error(tk, std::format(R"( cannot do operator {} for type {} )",
					tk.lexeme(),
					left->printable_string())),
			false,
			false);
}

type_compatibility
resolver::check_type_postfix_expression(const clox::scanning::token& tk, const shared_ptr<lox_type>& r)
{
	shared_ptr<lox_type> right{ r };

	if (lox_type::is_instance(*right))right = static_pointer_cast<lox_instance_type>(right)->underlying_type();

	switch (tk.type())
	{

	case scanning::token_type::PLUS_PLUS:
	case scanning::token_type::MINUS_MINUS:
		if (lox_type::is_primitive(*right))
		{
			auto possible_types = { lox_object_type::integer(), lox_object_type::floating() };

			for (const auto& t:possible_types)
			{
				if (lox_type::unify(*t, *right))
				{
					return make_tuple(t, true, false);
				}
			}
		}
	}

	return make_tuple(type_error(tk, std::format(R"( cannot do operator {} for type {} )",
					tk.lexeme(),
					right->printable_string())),
			false,
			false);
}


type_compatibility
resolver::check_type_ternary_expression(const clox::scanning::token& tk, const shared_ptr<lox_type>& l,
		const shared_ptr<lox_type>& r)
{
	shared_ptr<lox_type> left{ l }, right{ r };

	// FIXME: do not do this. handle this in lox_type
	if (lox_type::is_instance(*left))left = static_pointer_cast<lox_instance_type>(left)->underlying_type();
	if (lox_type::is_instance(*right))right = static_pointer_cast<lox_instance_type>(right)->underlying_type();

	auto intersect = lox_type::intersect(left, right);
	if (intersect)
	{
		return make_tuple(intersect,
				false,
				false);
	}

	return make_tuple(type_error(tk, std::format(R"( cannot do operator {} for type {} and {} )",
					tk.lexeme(),
					left->printable_string(),
					right->printable_string())),
			false,
			false);
}

type_compatibility
resolver::check_type_logical_expression(const clox::scanning::token& tk, const shared_ptr<lox_type>& l,
		const shared_ptr<lox_type>& r)
{
	shared_ptr<lox_type> left{ l }, right{ r };

	if (lox_type::is_instance(*left))left = static_pointer_cast<lox_instance_type>(left)->underlying_type();
	if (lox_type::is_instance(*right))right = static_pointer_cast<lox_instance_type>(right)->underlying_type();

	auto compatible =
			lox_type::unify(*lox_object_type::boolean(), *left) && lox_type::unify(*lox_object_type::boolean(), *right);

	if (compatible)
	{
		return make_tuple(lox_object_type::boolean(), compatible, false);
	}

	return make_tuple(type_error(tk, std::format(R"( cannot do operator {} for type {} and {} )",
					tk.lexeme(),
					left->printable_string(),
					right->printable_string())),
			false,
			false);
}


