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
// Created by cleve on 6/9/2021.
//

#pragma once

#include <parser/gen/parser_classes.inc>

#include <string>

namespace clox::parsing
{
class [[maybe_unused]] ast_printer final : public expression_visitor<std::string>
{
public:
	std::string to_string(const expression& expr);

	std::string visit_binary_expression(const std::shared_ptr<binary_expression>& expression) override;

	std::string visit_unary_expression(const std::shared_ptr<unary_expression>& expression) override;

	std::string visit_literal_expression(const std::shared_ptr<literal_expression>& expression) override;

	std::string visit_grouping_expression(const std::shared_ptr<grouping_expression>& expression) override;

private:
	std::string parenthesize(const std::string& name, std::initializer_list<expression*> exprs);

};
}