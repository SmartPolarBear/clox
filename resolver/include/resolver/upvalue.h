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
// Created by cleve on 10/2/2021.
//

#pragma once

#include <resolver/symbol.h>

#include <variant>

namespace clox::resolving
{

class upvalue final
{
public:
	friend class function_scope;

	using index_type = int64_t;

	static inline constexpr index_type INVALID_INDEX = -1;

	using symbol_type = std::shared_ptr<symbol>;
	using upvalue_type = std::shared_ptr<upvalue>;
public:

	explicit upvalue(symbol_type symbol)
			: capture_object_(symbol)
	{
	}

	explicit upvalue(upvalue_type upvalue)
			: capture_object_(upvalue)
	{
	}

	[[nodiscard]] bool holds_symbol() const
	{
		return std::holds_alternative<symbol_type>(capture_object_);
	}

	[[nodiscard]] bool holds_upvalue() const
	{
		return std::holds_alternative<upvalue_type>(capture_object_);
	}

	template<typename T>
	T& get_object()
	{
		return get<T>(capture_object_);
	}

	[[nodiscard]] index_type access_index()
	{
		return std::visit([](auto&& val) -> index_type
		{
			using T = std::decay_t<decltype(val)>;
			if constexpr(std::is_same_v<T, symbol_type>)
			{
				return downcast_symbol<named_symbol>(val)->slot_index();
			}
			else
			{
				return val->index_;
			}
		}, capture_object_);
	}

private:
	std::variant<symbol_type, upvalue_type> capture_object_{};

	index_type index_{ INVALID_INDEX };
};

}

