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
// Created by cleve on 8/15/2021.
//

#pragma once

#include <resolver/symbol.h>

namespace clox::resolving
{
class symbol_table
{
public:
	template<typename TSymbol, class... Args>
	requires std::derived_from<TSymbol, symbol>
	void put(const std::shared_ptr<parsing::expression>& expr, Args&& ... args)
	{
		if (table_.contains(expr))
		{
			throw std::invalid_argument{ "expr is already in the table" };
		}

		auto attributes = std::make_shared<TSymbol>(std::forward<Args>(args)...);
		table_[expr] = attributes;
	}

	[[nodiscard]] std::shared_ptr<symbol> at(const std::shared_ptr<parsing::expression>& expr);

	[[nodiscard]] bool contains(const std::shared_ptr<parsing::expression>& expr) const;

private:
	std::unordered_map<std::shared_ptr<parsing::expression>, std::shared_ptr<symbol>> table_{};
};
}
