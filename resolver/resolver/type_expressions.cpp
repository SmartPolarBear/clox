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

#include "type/lox_type.h"

#include "type/object_type.h"
#include "type/callable_type.h"
#include "type/union_type.h"
#include "type/instance_type.h"
#include "type/array_type.h"
#include "type/list_type.h"
#include "type/map_type.h"

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

shared_ptr<lox_type> resolver::visit_union_type_expression(const std::shared_ptr<struct union_type_expression>& ute)
{
	auto left = resolve(ute->get_left());
	auto right = resolve(ute->get_right());

	return lox_union_type::unite(left, right);
}

//shared_ptr<lox_type> resolver::visit_array_type_expression(const std::shared_ptr<struct array_type_expression>& ate)
//{
//	auto element_type = resolve(ate->get_element_type());
//
//	return make_shared<lox_array_type>(element_type);
//}

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

shared_ptr<lox_type> resolver::visit_list_type_expression(const std::shared_ptr<list_type_expression>& lte)
{
	auto element_type = resolve(lte->get_element_type());

	return make_shared<lox_list_type>(element_type);
}

shared_ptr<lox_type> resolver::visit_map_type_expression(const std::shared_ptr<map_type_expression>& mte)
{
	auto key_type = resolve(mte->get_key_type());
	auto val_type = resolve(mte->get_value_type());

	return make_shared<lox_map_type>(key_type, val_type);
}
