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
// Created by cleve on 9/6/2021.
//

#pragma once

#include <scanner/scanner.h>

#include <interpreter/vm/gc_recyclable.h>

#include <variant>
#include <string>
#include <string_view>

#include <memory>
#include <map>

namespace clox::interpreting::vm
{
enum class object_type
{
	OBJECT_TYPE_MIN,

	STRING,
	FUNCTION,
	CLOSURE,
	UPVALUE,

	OBJECT,
	INSTANCE,

	OBJECT_TYPE_MAX,
};


class object
		: public helper::printable,
		  public gc_recyclable
{

public:
	static inline bool is_string(const object& obj)
	{
		return obj.type() == object_type::STRING;
	}

	static bool pointer_equal(const object* lhs, const object* rhs);


public:
	[[nodiscard]]virtual object_type type() const noexcept = 0;
};

/// \brief object raw pointer will be used frequently because memory reclaim will be done by GC
using object_raw_pointer = object*;

template<class T>
concept object_pointer=
requires(T t){
	std::is_pointer_v<T>;
	std::derived_from<std::decay_t<decltype(*t)>, object>;
};

}

#include <magic_enum.hpp>

namespace magic_enum
{
template<>
struct customize::enum_range<clox::interpreting::vm::object_type>
{
	static constexpr int min = (int)clox::interpreting::vm::object_type::OBJECT_TYPE_MIN;
	static constexpr int max = (int)clox::interpreting::vm::object_type::OBJECT_TYPE_MAX;
};
}

#include <string>
#include <format>

namespace std
{
template<>
struct std::formatter<clox::interpreting::vm::object_type> : std::formatter<std::string>
{
	auto format(clox::interpreting::vm::object_type ot, format_context& ctx)
	{
		std::string str{ magic_enum::enum_name(ot) };
		return formatter<string>::format(
				str, ctx);
	}
};
}