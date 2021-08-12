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
// Created by cleve on 8/8/2021.
//

#pragma once

#include <resolver/lox_type.h>
#include <resolver/object_type.h>

#include <parser/parser.h>

#include <vector>
#include <variant>

namespace clox::resolving
{


class lox_callable_type
		: public lox_object_type,
		  public std::enable_shared_from_this<lox_callable_type>
{
public:
	using param_list_type = std::vector<std::pair<scanning::token, std::shared_ptr<lox_type>>>;
	using return_type_variant = std::variant<type_deduce_defer_tag, std::shared_ptr<lox_type>>;
public:

	[[nodiscard]] lox_callable_type(std::string name, return_type_variant return_type,
			param_list_type params, bool ctor = false);

	std::string printable_string() override;

	[[nodiscard]] bool is_ctor() const
	{
		return lox_object_type::flags() & FLAG_CTOR;
	}

	bool operator<(const lox_type& target) const override;

	bool operator==(const lox_type& lox_type) const override;

	bool operator!=(const lox_type& lox_type) const override;

	auto param_size() const
	{
		return params_.size();
	}

	std::shared_ptr<lox_type> param_type(size_t i) const
	{
		return params_.at(i).second;
	}

	param_list_type params() const
	{
		return params_;
	}

	[[nodiscard]] bool return_type_deduced() const;

	void set_return_type(const std::shared_ptr<lox_type>&);

	std::shared_ptr<lox_type> return_type() const;

private:
	std::string name_{};

	return_type_variant return_type_{};
	param_list_type params_{};
};

}
