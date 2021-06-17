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
// Created by cleve on 6/14/2021.
//

#pragma once

#include <base.h>

#include <parser/gen/parser_classes.inc>
#include <scanner/nil_value.h>

#include <parser/parser.h>

#include <variant>
#include <string>

namespace clox::interpreting
{

using interpreting_result = std::variant<long double, bool, std::string, scanning::nil_value_tag_type>;

class interpreter final :
		public parsing::visitor<interpreting_result>,
		public base::singleton<interpreter>
{
public:


	void interpret(const std::shared_ptr<parsing::expression>& expr);

	interpreting_result visit_binary_expression(const std::shared_ptr<parsing::binary_expression>& expression) override;

	interpreting_result visit_unary_expression(const std::shared_ptr<parsing::unary_expression>& expression) override;

	interpreting_result visit_literal(const std::shared_ptr<parsing::literal>& expression) override;

	interpreting_result visit_grouping(const std::shared_ptr<parsing::grouping>& expression) override;

private:
	interpreting_result evaluate(const std::shared_ptr<parsing::expression>& expr);

	static std::string result_to_string(const interpreting_result& res);

	static constexpr std::string_view bool_to_string(bool b);

	static void check_numeric_operands(scanning::token, const interpreting_result& l, const interpreting_result& r);

	static interpreting_result literal_value_to_interpreting_result(std::any any);

	static bool is_equal(interpreting_result lhs, interpreting_result rhs);

	static bool is_truthy(interpreting_result res);
};
}
