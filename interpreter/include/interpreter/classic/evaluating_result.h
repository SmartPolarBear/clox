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
// Created by cleve on 6/19/2021.
//
#pragma once

#include <scanner/scanner.h>

#include <parser/gen/parser_classes.inc>

#include <variant>
#include <string>

#include <memory>
#include <map>

namespace clox::interpreting::classic
{

using overloaded_functions = std::unordered_map<std::shared_ptr<parsing::statement>, std::shared_ptr<class callable>>;

using evaluating_result = std::variant<
		scanning::integer_literal_type,
		scanning::floating_literal_type,
		scanning::boolean_literal_type,
		scanning::string_literal_type,
		scanning::nil_value_tag_type,
		overloaded_functions,
		std::shared_ptr<class callable>,
		std::shared_ptr<class lox_instance>,
		std::shared_ptr<class lox_initializer_list>>;

class callable
{
public:
	virtual evaluating_result
	call(struct interpreter* the_interpreter, const std::shared_ptr<parsing::expression>& caller,
			const std::vector<evaluating_result>& args) = 0;
};

/// Use by std::visit
class evaluating_result_stringify_visitor
{
public:

	~evaluating_result_stringify_visitor() = default;

	explicit evaluating_result_stringify_visitor(const scanning::token& error_token) :
			error_token_(error_token)
	{
	}

	std::string operator()(scanning::integer_literal_type integer);

	std::string operator()(scanning::floating_literal_type floating);

	std::string operator()(scanning::boolean_literal_type boolean);

	std::string operator()(scanning::string_literal_type string_lit);

	std::string operator()(scanning::nil_value_tag_type nil_value);

	std::string operator()(const std::shared_ptr<class lox_instance>& inst);

	std::string operator()(const std::shared_ptr<class callable>& callable);

	std::string operator()(const std::shared_ptr<class lox_initializer_list>& lst);

	std::string operator()(overloaded_functions overload_func);

private:
	static inline constexpr std::string_view bool_to_string(bool b)
	{
		return b ? "true" : "false";
	}

	scanning::token error_token_;
};


}