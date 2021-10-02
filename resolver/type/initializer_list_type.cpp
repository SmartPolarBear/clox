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
// Created by cleve on 8/29/2021.
//

#include <resolver/initializer_list_type.h>
#include <resolver/array_type.h>

#include <gsl/gsl>
#include <utility>

using namespace std;
using namespace std::ranges;

using namespace gsl;

using namespace clox::resolving;

std::string clox::resolving::initializer_list_type::printable_string()
{
	std::string ret{};
	for (const auto& item: items_)
	{
		ret += format("{},", item->printable_string());
	}
	*ret.rbegin() = ' ';

	return format(R"(initializer list of {{{}}})", ret);
}

uint64_t clox::resolving::initializer_list_type::flags() const
{
	return TYPE_PRIMITIVE;
}

clox::resolving::type_id clox::resolving::initializer_list_type::id() const
{
	return TYPE_ID_INITIALIZER_LIST;
}

bool clox::resolving::initializer_list_type::operator<(const clox::resolving::lox_type& target) const
{
	if (target.id() == TYPE_ID_ARRAY_CLASS)
	{
		auto array_type = dynamic_cast<const lox_array_type&>(target);
		return ranges::all_of(items_, [&array_type](const auto& item)
		{
			return *item < *array_type.element_type();
		});
	}

	return false;
}

bool clox::resolving::initializer_list_type::operator==(const clox::resolving::lox_type& another) const
{
	if (another.id() != TYPE_ID_INITIALIZER_LIST)
	{
		return false;
	}

	auto another_list = dynamic_cast<const clox::resolving::initializer_list_type&>(another);

	return items_ == another_list.items_;
}

bool clox::resolving::initializer_list_type::operator!=(const clox::resolving::lox_type& another) const
{
	return !operator==(another);
}

initializer_list_type::initializer_list_type(std::vector<std::shared_ptr<lox_type>> items)
		: items_(std::move(items))
{
}
