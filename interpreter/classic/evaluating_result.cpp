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
// Created by cleve on 8/30/2021.
//

#include <interpreter/classic/evaluating_result.h>
#include <interpreter/classic/lox_instance.h>
#include <interpreter/classic/initializer_list.h>

#include <format>

using namespace std;
using namespace clox::interpreting::classic;

std::string
clox::interpreting::classic::evaluating_result_stringify_visitor::operator()(clox::scanning::integer_literal_type integer)
{
	return to_string((clox::scanning::floating_literal_type) /*FIXME*/ integer);
}

std::string
clox::interpreting::classic::evaluating_result_stringify_visitor::operator()(clox::scanning::floating_literal_type floating)
{
	return to_string(floating);
}

std::string
clox::interpreting::classic::evaluating_result_stringify_visitor::operator()(clox::scanning::boolean_literal_type boolean)
{
	return string{ bool_to_string(boolean) };
}

std::string
clox::interpreting::classic::evaluating_result_stringify_visitor::operator()(clox::scanning::string_literal_type string_lit)
{
	return string_lit;
}

std::string
clox::interpreting::classic::evaluating_result_stringify_visitor::operator()(clox::scanning::nil_value_tag_type nil_value)
{
	return "nil";
}

std::string
clox::interpreting::classic::evaluating_result_stringify_visitor::operator()(const std::shared_ptr<lox_instance>& inst)
{
	return inst->printable_string();
}

std::string
clox::interpreting::classic::evaluating_result_stringify_visitor::operator()(const std::shared_ptr<callable>& cl)
{
	auto printable = dynamic_pointer_cast<helper::printable>(cl);
	if (!printable)
	{
		throw clox::interpreting::classic::runtime_error{ error_token_, std::format("{} is not printable.",
				typeid(cl).name()) };
	}
	return printable->printable_string();
}

std::string clox::interpreting::classic::evaluating_result_stringify_visitor::operator()(
		clox::interpreting::classic::overloaded_functions overload_func)
{
	throw clox::interpreting::classic::runtime_error{ error_token_, std::format("{} is not printable.",
			typeid(overload_func).name()) };
}

std::string clox::interpreting::classic::evaluating_result_stringify_visitor::operator()(
		const shared_ptr<lox_initializer_list>& lst)
{
	throw clox::interpreting::classic::runtime_error{ error_token_, std::format("{} is not printable.",
			typeid(lst).name()) };
}
