// Copyright (c) 2022 SmartPolarBear
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
// Created by cleve on 8/27/2021.
//

#pragma once


#include "lox_type.h"
#include "object_type.h"

#include "parser/parser.h"

#include <vector>
#include <variant>
#include <map>
#include <optional>
#include <stdexcept>

namespace clox::resolving
{

class lox_union_type final
		: public lox_type
{
public:
	using children_list_type = std::vector<std::shared_ptr<lox_type>>;

public:
	explicit lox_union_type(children_list_type children);

	auto begin()
	{
		return children_.begin();
	}

	auto end()
	{
		return children_.end();
	}


	std::string printable_string() override;

	[[nodiscard]] uint64_t flags() const override;

	[[nodiscard]] type_id id() const override;

	bool operator<(const lox_type& target) const override;

	bool operator==(const lox_type& lox_type) const override;

	bool operator!=(const lox_type& lox_type) const override;

	static std::shared_ptr<lox_union_type>
	unite(const std::shared_ptr<lox_type>& left, const std::shared_ptr<lox_type>& right);

private:
	children_list_type children_{};
};
}