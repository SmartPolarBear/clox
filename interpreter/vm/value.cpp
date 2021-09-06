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
// Created by cleve on 9/4/2021.
//

#include <interpreter/vm/value.h>
#include <interpreter/vm/exceptions.h>

using namespace std;

using namespace clox::interpreting;
using namespace clox::interpreting::vm;

clox::interpreting::vm::value_stringify_visitor::value_stringify_visitor(bool show_type)
		: show_type_(show_type)
{
}


template<>
std::string clox::interpreting::vm::value_stringify_visitor::operator()(clox::scanning::nil_value_tag_type val)
{
	return std::format("{} {}", type_name_of<std::decay_t<decltype(val)>>(), "nil");
}


clox::scanning::floating_literal_type
clox::interpreting::vm::get_number_promoted(const clox::interpreting::vm::value& val)
{
	return std::visit([&val](auto&& v) -> scanning::floating_literal_type
	{
		using T = std::decay_t<decltype(v)>;
		if constexpr(std::is_same_v<T, scanning::floating_literal_type> ||
					 std::is_same_v<T, scanning::integer_literal_type> ||
					 std::is_same_v<T, scanning::boolean_literal_type>)
		{
			return static_cast<scanning::floating_literal_type>(v);
		}
		else
		{
			throw invalid_value{ val };
		}

	}, val);
}
