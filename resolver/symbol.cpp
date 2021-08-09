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
// Created by cleve on 8/4/2021.
//

#include <resolver/symbol.h>

using namespace clox::resolving;

std::shared_ptr<symbol> clox::resolving::symbol_table::at(const std::shared_ptr<parsing::expression>& expr)
{
	return table_.at(expr);
}

bool symbol_table::contains(const std::shared_ptr<parsing::expression>& expr) const
{
	return table_.contains(expr);
}

void symbol_table::set_depth(const std::shared_ptr<parsing::expression>& expr, int64_t d)
{
	if (contains(expr))
	{
		at(expr)->set_depth(d);
	}
	else
	{
		put(expr, d, nullptr);
	}
}

void symbol_table::set_type(const std::shared_ptr<parsing::expression>& expr, const std::shared_ptr<lox_type>& type)
{
	if (contains(expr))
	{
		at(expr)->set_type(type);
	}
	else
	{
		put(expr, symbol::INVALID_DEPTH, type);
	}
}