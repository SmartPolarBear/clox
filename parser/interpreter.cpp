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

#include <parser/interpreter.h>

clox::parsing::interpreting_result
clox::parsing::interpreter::visit_binary_expression(clox::parsing::binary_expression* expression)
{
	return clox::parsing::interpreting_result();
}

clox::parsing::interpreting_result
clox::parsing::interpreter::visit_unary_expression(clox::parsing::unary_expression* expression)
{
	return clox::parsing::interpreting_result();
}

clox::parsing::interpreting_result clox::parsing::interpreter::visit_literal(clox::parsing::literal* literal)
{
}

clox::parsing::interpreting_result clox::parsing::interpreter::visit_grouping(clox::parsing::grouping* grouping)
{
	return clox::parsing::interpreting_result();
}

std::string clox::parsing::interpreter::result_to_string(const clox::parsing::interpreting_result& res)
{
	return std::string();
}

void clox::parsing::interpreter::interpret(const clox::parsing::expression& expr)
{

}
