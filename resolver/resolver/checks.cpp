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
	case scanning::token_type::MINUS:
	case scanning::token_type::STAR:
	case scanning::token_type::SLASH:
	{
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
	}

	case scanning::token_type::PLUS:
	{
		auto possible_types = { lox_object_type::boolean(), lox_object_type::integer(),
								lox_object_type::floating(), lox_object_type::string() };

		for (const auto& t:possible_types) // not  call intersect for extensibility
		{
			if (lox_type::unify(*t, *left) && lox_type::unify(*t, *right))
			{
				return make_tuple(t, true, false);
			}
		}
		break;
	}

	case scanning::token_type::LESS:
	case scanning::token_type::GREATER:
	case scanning::token_type::LESS_EQUAL:
	case scanning::token_type::GREATER_EQUAL:
		if (lox_type::is_primitive(*left) && lox_type::is_primitive(*right))
		{
			auto possible_types = { lox_object_type::boolean(), lox_object_type::integer(),
									lox_object_type::floating(), lox_object_type::string() };

			for (const auto& t:possible_types) // not  call intersect for extensibility
			{
				if (lox_type::unify(*t, *left) && lox_type::unify(*t, *right))
				{
					return make_tuple(lox_object_type::boolean(), true, false);
				}
			}
		}

	case scanning::token_type::BANG_EQUAL:
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
