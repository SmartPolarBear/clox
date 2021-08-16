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
// Created by cleve on 8/14/2021.
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


shared_ptr<lox_type> resolver::resolve_function_decl(const shared_ptr<function_statement>& func)
{
	bool ctor = false;
	if (cur_class_.top() != env_class_type::CT_NONE)
	{
		if (func->get_func_type() == parsing::function_statement_type::FST_CTOR)
		{
			ctor = true;
		}
	}

	lox_callable_type::return_type_variant ret_{ type_deduce_defer };
	if (!ctor && func->get_return_type_expr())
	{
		ret_ = resolve(func->get_return_type_expr());
	}
	else if (ctor && func->get_return_type_expr())
	{
		return type_error(func->get_name(), "An initializer of class cannot have a explicit return type");
	}

	vector<pair<scanning::token, shared_ptr<lox_type>>> params_{};
	for (const auto& param:func->get_params())
	{
		shared_ptr<lox_type> param_type{ resolve(param.second) };
		if (lox_type::is_class(*param_type))
		{
			param_type = make_shared<lox_instance_type>(static_pointer_cast<lox_object_type>(param_type));
		}

		params_.emplace_back(param.first, param_type);
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


void resolver::visit_class_statement(const std::shared_ptr<class_statement>& cls)
{
	cur_class_.push(env_class_type::CT_CLASS);

	auto[class_type, base_class_type, this_type] = resolve_class_type_decl(cls);

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
	define_name(cls->get_name(), class_type);
	// have already call define_type(cls->get_name(), class_type); in resolve_class_type_decl
	// to support fields with this class type or relevant function return type

	scope_begin();
	declare_name("base", cls->get_name());
	define_name("base", base_class_type); // base is not an instance.

	scope_begin();
	declare_name("this", cls->get_name());
	define_name("this", this_type);

	auto _ = finally([this]
	{
		this->scope_end();
		this->scope_end();

		this->cur_class_.pop();
	});

	resolve_class_members(cls, class_type);
}


std::tuple<shared_ptr<lox_class_type>,
		std::shared_ptr<lox_class_type>,
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

	define_type(cls->get_name(), this_type);

	for (const auto& field:cls->get_fields())
	{
		this_type->fields()[field->get_name().lexeme()] = resolve(field->get_type_expr());
	}

	for (const auto& method:cls->get_methods())
	{
		auto type = resolve_function_decl(method);
		if (type->id() == PRIMITIVE_TYPE_ID_ANY)
		{
			continue;
		}
		this_type->methods()[method->get_name().lexeme()] = static_pointer_cast<lox_callable_type>(type);
	}

	complement_default_members(cls, this_type);

	return { static_pointer_cast<lox_class_type>(this_type),
			 static_pointer_cast<lox_class_type>(base_type),
			 make_shared<lox_instance_type>(static_pointer_cast<lox_class_type>(this_type)) };
}


void resolver::complement_default_members(const shared_ptr<parsing::class_statement>& cls,
		const shared_ptr<lox_class_type>& class_type)
{
	if (!class_type->methods().contains(scanning::scanner::keyword_from_type(scanning::token_type::CONSTRUCTOR)))
	{
		class_type->methods()[scanning::scanner::keyword_from_type(
				scanning::token_type::CONSTRUCTOR)] = make_shared<lox_callable_type>(
				scanning::scanner::keyword_from_type(scanning::token_type::CONSTRUCTOR), class_type,
				lox_callable_type::param_list_type{}, true);
	}
}


void resolver::resolve_class_members(const shared_ptr<parsing::class_statement>& cls,
		const std::shared_ptr<lox_class_type>& class_type)
{
	for (const auto& field:cls->get_fields())
	{
		resolve(field);
	}

	for (const auto& method:cls->get_methods())
	{
		auto decl = env_function_type::FT_METHOD;
		if (method->get_func_type() == parsing::function_statement_type::FST_CTOR)
		{
			decl = env_function_type::FT_CTOR;
		}

		auto func_type = class_type->methods()[method->get_name().lexeme()];

		cur_func_type_.push(func_type);

		auto _ = gsl::finally([this]
		{
			cur_func_type_.pop();
		});

		resolve_function_body(method, decl);

		if (!func_type->return_type_deduced())
		{
			if (decl == env_function_type::FT_CTOR)
			{
				func_type->set_return_type(class_type);
			}
			else
			{
				func_type->set_return_type(make_shared<lox_void_type>());
			}
		}
	}
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
		var_type = make_shared<lox_instance_type>(static_pointer_cast<lox_object_type>(var_type));
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

	if (stmt->get_func_type() == function_statement_type::FST_OPERATOR)
	{
		// TODO
	}

	auto type = resolve_function_decl(stmt);
	if (type->id() == PRIMITIVE_TYPE_ID_ANY)
	{
		return;
	}

	auto func_type = static_pointer_cast<lox_callable_type>(type);

	define_name(stmt->get_name(), stmt, func_type); // use special define_name

	cur_func_type_.push(func_type);

	auto _ = gsl::finally([this]
	{
		cur_func_type_.pop();
	});

	resolve_function_body(stmt, env_function_type::FT_FUNCTION);

	if (!func_type->return_type_deduced())
	{
		func_type->set_return_type(make_shared<lox_void_type>());
	}
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

