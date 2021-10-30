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
		global_scope_{ std::make_shared<function_scope>(nullptr, nullptr, FUNCTION_ID_GLOBAL, global_scope_tag) },
		bindings_(make_shared<binding_table>())
{
	global_scope_->types()["void"] = make_shared<lox_void_type>();

	global_scope_->types()["object"] = lox_object_type::object();

	global_scope_->types()["integer"] = lox_object_type::integer();
	global_scope_->types()["floating"] = lox_object_type::floating();
	global_scope_->types()["boolean"] = lox_object_type::boolean();
	global_scope_->types()["nil"] = lox_object_type::nil();

	global_scope_->types()["string"] = lox_object_type::string();

	scopes_.push(global_scope_);

	function_scopes_.push(global_scope_);
	function_scope_ids_[FUNCTION_ID_GLOBAL] = global_scope_;

	cur_func_id_.push(FUNCTION_ID_GLOBAL);

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
	for (const auto& stmt: stmts)
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
//	scope_begin(scopes_.top()->container_function());
	auto next = make_shared<scope>(scopes_.top(), scopes_.top()->container_function());

	scopes_.top()->children_.push_back(next);

	scopes_.push(next);
}

void resolver::scope_begin(function_id_type func_id)
{
	auto next = make_shared<function_scope>(scopes_.top(), function_scopes_.top(), func_id);

	scopes_.top()->children_.push_back(next);

	scopes_.push(next);

	// it is a new function scoop

	function_scopes_.top()->child_functions_.push_back(next);

	function_scopes_.push(next);

	function_scope_ids_[func_id] = next;
}

void resolver::scope_begin(const shared_ptr<lox_class_type>& class_type, class_base_tag)
{
	auto next = make_shared<class_base_scope>(scopes_.top(), class_type);

	scopes_.top()->children_.push_back(next);

	scopes_.push(next);
}

void resolver::scope_begin(const shared_ptr<lox_class_type>& class_type, class_field_tag)
{
	auto next = make_shared<class_field_scope>(scopes_.top(), class_type);

	scopes_.top()->children_.push_back(next);

	scopes_.push(next);
}


void resolver::scope_end()
{
	auto top = scopes_.top();
	slots_in_use_ -= top->names().size();

	scopes_.pop();

}

void resolver::declare_name(const clox::scanning::token& t, size_t dist)
{
	declare_name(t.lexeme(), t, dist);
}


void resolver::declare_name(const string& lexeme, const clox::scanning::token& error_tk, size_t dist)
{
	if (scopes_.empty())return;

	auto top = scopes_.peek(dist);

	if (top->contains_name(lexeme))
	{
		logger::instance().error(error_tk, std::format("{} already exists in this scoop.", lexeme));
	}

	top->names()[lexeme] = nullptr; // initialize a slot, this avoiding using operator[] in following codes makes error in code reveals quicker.
}


function_id_type resolver::declare_function(const shared_ptr<function_statement>& fs, size_t dist)
{
	if (scopes_.empty())return FUNCTION_ID_INVALID;

	auto next_id = next_function_id(fs->get_name());
	if (next_id == FUNCTION_ID_INVALID)
	{
		return next_id; // TODO: throw an exception
	}

	auto top = scopes_.peek(dist);

	if (!top->contains_name(fs->get_name().lexeme()))
	{
		top->names()[fs->get_name().lexeme()] = nullptr; // initialize a slot, this avoiding using operator[] in following codes makes error in code reveals quicker.
	}

	if (!function_ids_.contains(fs))
	{
		function_ids_.insert_or_assign(fs, next_id);
	}

	return next_id;
}


void resolver::define_name(const clox::scanning::token& tk, const std::shared_ptr<lox_type>& type, size_t dist)
{
	define_name(tk.lexeme(), type, dist);
}

void resolver::define_name(const string& tk, const shared_ptr<lox_type>& type, size_t dist)
{
	if (scopes_.empty())return;

	auto target = scopes_.peek(dist);

	if (target->is_global())
	{
		target->names().at(tk) = make_shared<named_symbol>(tk, type);
	}
	else
	{
		target->names().at(tk) = make_shared<named_symbol>(tk, type, named_symbol::named_symbol_type::LOCAL,
				slots_in_use_++);
	}
}


void resolver::define_type(const clox::scanning::token& tk, const shared_ptr<lox_type>& type, uint64_t dist)
{
	if (lox_type::is_instance(*type))
	{
		throw invalid_argument{ "type" };
	}

	if (scopes_.empty())return;
	scopes_.peek(dist)->types()[tk.lexeme()] = type;
}


std::shared_ptr<upvalue>
resolver::resolve_upvalue(const shared_ptr<function_scope>& cur, const shared_ptr<function_scope>& bottom,
		const shared_ptr<symbol>& sym)
{
	if (auto parent = cur->parent_function_.lock();parent == bottom)
	{
		if (auto named = downcast_symbol<named_symbol>(sym);named->is_captured())
		{
			return named->get_upvalue();
		}
		else
		{
			return cur->put_upvalue(named->capture());
		}
	}

	auto pa = cur->parent_function_.lock();
	auto new_upvalue = cur->put_upvalue(make_shared<upvalue>(resolve_upvalue(pa, bottom, sym)));

	return new_upvalue;
}

std::shared_ptr<symbol> resolver::resolve_local(const shared_ptr<expression>& expr, const clox::scanning::token& tk)
{
	int64_t depth = 0;

	for (const auto& s: scopes_ | views::reverse) // traverse from the stack top, which has a depth of zero.
	{
		if (s->contains_name(tk.lexeme()))
		{
			auto ret = s->name_typed<named_symbol>(tk.lexeme());

			if (s->container_function() != FUNCTION_ID_GLOBAL &&
				s->container_function() != scopes_.top()->container_function())
			{
				auto top_function = function_scope_ids_[scopes_.top()->container_function()], bottom_function = function_scope_ids_[s->container_function()];
				auto upvalue = resolve_upvalue(top_function, bottom_function, ret);
				bindings_->put<variable_binding>(expr, expr, depth, ret, upvalue);
			}
			else
			{
				bindings_->put<variable_binding>(expr, expr, depth, ret);
			}


			return ret;
		}
		depth++;
	}

	return nullptr;
}


std::shared_ptr<lox_type> resolver::type_lookup(const scanning::token& tk)
{
	for (auto& scoop: scopes_)
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
	auto s = scopes_.peek(dist);

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

	for (const auto& arg: call->get_args())
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

	if (stmt)
	{
		auto func_id = function_ids_.at(stmt);
		bindings_->put<function_binding>(call, static_pointer_cast<call_expression>(call),
				static_pointer_cast<statement>(stmt), func_id);
	}
	else // it's a constructor
	{
		auto class_type = callable->return_type();
		bindings_->put<function_binding>(call, static_pointer_cast<call_expression>(call),
				static_pointer_cast<statement>(stmt), FUNCTION_ID_DEFAULT_CTOR, callable->flags() & FLAG_CTOR,
				static_pointer_cast<lox_class_type>(class_type));
	}


	return callable;
}

std::shared_ptr<binding_table> resolver::bindings() const
{
	return bindings_;
}

std::shared_ptr<binding> resolver::binding(const shared_ptr<parsing::expression>& e) const
{
	if (!bindings_->contains(e))
	{
		return nullptr;
	}

	return bindings_->get(e);
}

optional<function_id_type> resolver::function_id(const shared_ptr<parsing::statement>& stmt) const
{
	if (function_ids_.contains(stmt))
	{
		return function_ids_.at(stmt);
	}

	return std::nullopt;
}


