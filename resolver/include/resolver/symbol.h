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

#pragma once

#include <scanner/scanner.h>

#include <parser/parser.h>

#include <resolver/lox_type.h>

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <unordered_map>
#include <compare>

namespace clox::resolving
{
class symbol
{
public:
	static inline constexpr size_t INVALID_DEPTH = -1;
public:
	[[nodiscard]] explicit symbol(int64_t depth = INVALID_DEPTH,
			std::shared_ptr<lox_type> type = nullptr)
			: depth_(depth), type_(std::move(type))
	{
	}

	[[nodiscard]] int64_t depth() const
	{
		return depth_;
	}

	void set_depth(int64_t d)
	{
		depth_ = d;
	}

	[[nodiscard]] std::shared_ptr<lox_type> type() const
	{
		return type_;
	}

	auto operator<=>(const symbol& another) const = default;

private:
	int64_t depth_{};
	std::shared_ptr<lox_type> type_{};
};

class symbol_table
{
public:
	template<class... Args>
	void put(const std::shared_ptr<parsing::expression>& expr, Args&& ... args)
	{
		auto attributes = std::make_shared<symbol>(std::forward<Args>(args)...);
		table_[expr] = attributes;
	}

	[[nodiscard]] std::shared_ptr<symbol> at(const std::shared_ptr<parsing::expression>& expr);

	[[nodiscard]] bool contains(const std::shared_ptr<parsing::expression>& expr) const;

private:
	std::unordered_map<std::shared_ptr<parsing::expression>, std::shared_ptr<symbol>> table_{};
};
}