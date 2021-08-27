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

std::shared_ptr<lox_type> resolver::visit_variable_type_expression(const std::shared_ptr<variable_type_expression>& vte)
{
	return type_lookup(vte->get_name());
}


shared_ptr<lox_type> resolver::visit_union_type_expression(const std::shared_ptr<struct union_type_expression>& ptr)
{
	return std::shared_ptr<lox_type>();
}

shared_ptr<lox_type> resolver::visit_array_type_expression(const std::shared_ptr<struct array_type_expression>& ptr)
{
	return std::shared_ptr<lox_type>();
}

shared_ptr<lox_type>
resolver::visit_callable_type_expression(const std::shared_ptr<struct callable_type_expression>& cte)
{
	lox_callable_type::param_list_type params{};
	for (const auto& param: cte->get_params())
	{
		params.emplace_back(nullopt, resolve(param));
	}

	auto callable = make_shared<lox_callable_type>(resolve(cte->get_return_type()),
			params,
			false);

	return callable;
}
