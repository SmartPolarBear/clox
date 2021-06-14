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

#include <parser/gen/parser_classes.inc>
#include <parser/nil_value.h>

#include <variant>
#include <string>

namespace clox::parsing
{

using interpreting_result = std::variant<long double, bool, std::string, nil_value_tag_type>;

class interpreter final :
		public visitor<interpreting_result>
{
public:
	interpreting_result visit_binary_expression(struct binary_expression* expression) override;

	interpreting_result visit_unary_expression(struct unary_expression* expression) override;

	interpreting_result visit_literal(struct literal* literal) override;

	interpreting_result visit_grouping(struct grouping* grouping) override;
};
}