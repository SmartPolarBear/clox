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

resolver::resolver() :
		global_scope_{ std::make_shared<scope>() },
		bindings_(make_shared<binding_table>())
{
	global_scope_->types()["void"] = make_shared<lox_void_type>();

	global_scope_->types()["object"] = lox_object_type::object();

	global_scope_->types()["integer"] = lox_object_type::integer();
	global_scope_->types()["floating"] = lox_object_type::floating();
	global_scope_->types()["boolean"] = lox_object_type::boolean();
	global_scope_->types()["nil"] = lox_object_type::nil();

	global_scope_->types()["string"] = lox_object_type::string();

	cur_func_.push(env_function_type::FT_NONE);
	cur_class_.push(env_class_type::CT_NONE);
}

std::shared_ptr<lox_type> resolver::type_error(const clox::scanning::token& tk, const std::string& msg)
{
	logger::instance().error(tk, msg);
	return make_shared<lox_any_type>();
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
	declare_name(t.lexeme(), t, dist);
}


void resolver::declare_name(const string& lexeme, const clox::scanning::token& error_tk, size_t dist)
{
	if (scopes_.empty())return;

	auto top = scope_top(dist);

	if (top->contains_name(lexeme))
	{
		logger::instance().error(error_tk, std::format("{} already exists in this scoop.", lexeme));
	}

	top->names()[lexeme] = nullptr; // initialize a slot, this avoiding using operator[] in following codes makes error in code reveals quicker.
}


void resolver::declare_function_name(const clox::scanning::token& t, size_t dist)
{
	if (scopes_.empty())return;

	auto top = scope_top(dist);

	if (!top->contains_name(t.lexeme()))
	{
		top->names()[t.lexeme()] = nullptr; // initialize a slot, this avoiding using operator[] in following codes makes error in code reveals quicker.
	}
}


void resolver::define_name(const clox::scanning::token& tk, const std::shared_ptr<lox_type>& type, size_t dist)
{
	define_name(tk.lexeme(), type, dist);
}

void resolver::define_name(const string& tk, const shared_ptr<lox_type>& type, size_t dist)
{
	if (scopes_.empty())return;

	scope_top(dist)->names().at(tk) = make_shared<named_symbol>(tk, type);
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
		if (s->contains_name(tk.lexeme()))
		{
			bindings_->put<variable_binding>(expr, expr, depth);
			return s->name(tk.lexeme());
		}
		depth++;
	}

	return nullptr;
}


std::shared_ptr<lox_type> resolver::type_lookup(const scanning::token& tk)
{
	for (auto& scoop:scopes_)
	{
		if (scoop->contains_type(tk.lexeme()))
		{
			return scoop->type(tk.lexeme());
		}
	}

	return type_error(tk, std::format("Type {} is not defined.", tk.lexeme()));
}


void resolver::define_function_name(const clox::scanning::token& tk, const shared_ptr<parsing::statement>& stmt,
		const shared_ptr<lox_callable_type>& type, size_t dist)
{
	define_function_name(tk.lexeme(), tk, stmt, type, dist);
}

void resolver::define_function_name(const string& lexeme, const clox::scanning::token& error_tk,
		const shared_ptr<parsing::statement>& stmt,
		const shared_ptr<lox_callable_type>& type, size_t dist)
{
	if (scopes_.empty())return;

	shared_ptr<lox_overloaded_metatype> metatype{ nullptr };
	auto s = scope_top(dist);

	if (s->names().at(lexeme))
	{
		metatype = dynamic_pointer_cast<lox_overloaded_metatype>(s->names().at(lexeme)->type());

		if (!metatype)
		{
			type_error(error_tk, std::format("function {} is conflict with variables.", type->printable_string()));
		}
	}
	else
	{
		metatype = make_shared<lox_overloaded_metatype>(lexeme);
		s->names()[lexeme] = make_shared<named_symbol>(lexeme, metatype);
	}

	try
	{
		metatype->put(stmt, type);
	}
	catch (const too_many_params& e)
	{
		type_error(error_tk, e.what());
	}
	catch (const redefined_symbol& e)
	{
		type_error(error_tk, e.what());
	}
}

std::shared_ptr<lox_type> resolver::resolve_function_call(const shared_ptr<parsing::call_expression>& call,
		const std::shared_ptr<lox_overloaded_metatype>& callee)
{
	vector<shared_ptr<lox_type>> args{};

	for (const auto& arg:call->get_args())
	{
		auto type = resolve(arg);
		args.push_back(type);
	}

	auto resolve_ret = callee->get(args);
	if (!resolve_ret.has_value())
	{
		return type_error(call->get_paren(), "Incompatible parameter type");
	}

	auto[stmt, callable]=resolve_ret.value();

	bindings_->put<function_binding>(call, static_pointer_cast<call_expression>(call),
			static_pointer_cast<statement>(stmt));

	return callable;
}
