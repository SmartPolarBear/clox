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
// Created by cleve on 8/27/2021.
//

#include <resolver/union_type.h>

using namespace std;

std::string clox::resolving::union_type::printable_string()
{
	string ret{};
	for (const auto& child: children_)
	{
		ret += std::format("{},", child->printable_string());
	}
	*ret.rbegin() = '\0';
	return ret;
}

uint64_t clox::resolving::union_type::flags() const
{
	return FLAG_UNION_TYPE;
}

clox::resolving::type_id clox::resolving::union_type::id() const
{
	return TYPE_ID_UNION;
}

bool clox::resolving::union_type::operator<(const clox::resolving::lox_type& target) const
{
	if (!lox_type::is_union(target))
	{
		return false;
	}

	auto target_union_type = dynamic_cast<const union_type&>(target);

	for (const auto& target_child: target_union_type.children_)
	{
		if (!ranges::any_of(children_, [&target_child](const auto& child)
		{
			return target_child < child;
		}))
		{
			return false;
		}
	}

	return true;
}

bool clox::resolving::union_type::operator==(const clox::resolving::lox_type& target) const
{
	if (!lox_type::is_union(target))
	{
		return false;
	}

	auto target_union_type = dynamic_cast<const union_type&>(target);

	for (const auto& target_child: target_union_type.children_)
	{
		if (!ranges::any_of(children_, [&target_child](const auto& child)
		{
			return target_child == child;
		}))
		{
			return false;
		}
	}

	return true;
}

bool clox::resolving::union_type::operator!=(const clox::resolving::lox_type& another) const
{
	return !this->operator==(another);
}
