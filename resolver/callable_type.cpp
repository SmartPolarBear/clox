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
#include <helper/enum.h>

#include <resolver/callable_type.h>

#include <tuple>
#include <utility>
#include <format>

using namespace clox;

using namespace clox::helper;

using namespace clox::resolving;

lox_callable_type::lox_callable_type(std::string name, std::shared_ptr<lox_type> return_type,
		std::vector<std::shared_ptr<lox_type>> params)
		: name_(std::move(name)),
		  return_type_(std::move(return_type)),
		  params_(std::move(params))
{
}


std::string lox_callable_type::printable_string()
{
	auto ret = std::format("<callable object {} -> {} ( >", name_, return_type_->printable_string());

	for (const auto& param:params_)
	{
		ret += std::format("{},", param->printable_string());
	}

	*ret.rbegin() = ')';
	return ret;
}

uint64_t lox_callable_type::flags() const
{
	return TYPE_CLASS | FLAG_CALLABLE ;
}

type_id lox_callable_type::id() const
{
	return TYPE_ID_CALLABLE;
}

bool lox_callable_type::operator<(const lox_type& target) const
{
	return false;
}

bool lox_callable_type::operator==(const lox_type& another) const
{
	if (!is_callable(another))return false;

	const auto& callable = dynamic_cast<const lox_callable_type&>(another);

	if (name_ != callable.name_)return false;

	if (params_.size() != callable.params_.size())return false;

	auto param_size = params_.size();
	for (decltype(param_size) i = 0; i < param_size; i++)
	{
		if (params_[i] != callable.params_[i])
		{
			return false;
		}
	}

	return true;
}

bool lox_callable_type::operator!=(const lox_type& another) const
{
	return !(*this == another);
}

