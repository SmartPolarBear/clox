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
// Created by cleve on 9/3/2021.
//
#pragma once

#include <scanner/scanner.h>

#include <interpreter/vm/object.h>

#include <variant>
#include <string>

#include <memory>
#include <map>
#include "string_object.h"


namespace clox::interpreting::vm
{

using integer_value_type = scanning::integer_literal_type;
using floating_value_type = scanning::floating_literal_type;
using boolean_value_type = scanning::boolean_literal_type;
using nil_value_type = scanning::nil_value_tag_type;
using object_value_type = object_raw_pointer;
using variable_name_type = std::string;

using value = std::variant<integer_value_type,
		floating_value_type,
		boolean_value_type,
		nil_value_type,
		variable_name_type, // only for variable names
		object_value_type>;

static inline bool is_string_value(const value& val)
{
	return std::visit([](auto&& val) -> bool
	{
		using T = std::decay_t<decltype(val)>;

		if constexpr(std::is_same_v<T, object_raw_pointer>)
		{
			return object::is_string(*val);
		}
		else
		{
			return false;
		}
	}, val);
}

static inline bool operator==(const value& lhs, const value& rhs)
{
	return std::visit([&rhs](auto&& lhs_val) -> bool
	{
		using TLeft = std::decay_t<decltype(lhs_val)>;
		if constexpr(std::is_same_v<TLeft, nil_value_type>)
		{
			return std::holds_alternative<scanning::nil_value_tag_type>(rhs); // nil values always equal
		}
		else
		{
			return std::visit([&lhs_val](auto&& rhs_val) -> bool
			{
				using TRight = std::decay_t<decltype(rhs_val)>;
				if constexpr(!std::is_same_v<TLeft, TRight>)
				{
					return false; // not of the same type, they can't be equal
				}
				else
				{
					return lhs_val == rhs_val; // compare value of same type
				}
			}, rhs);
		}
	}, lhs);
}

/// Get number from value, promoting to floating type
/// \param val the value variant
/// \return numeric value promoted to floating type
/// \throws invalid_value
floating_value_type get_number_promoted(const value& val);

string_object_raw_pointer get_string(const value& val);

class value_stringify_visitor
{
public:
	value_stringify_visitor() = default;

	explicit value_stringify_visitor(bool show_type);

	template<typename T>
	std::string operator()(T val)
	{
		if constexpr(std::is_same_v<object_value_type, std::decay_t<T>>)
		{
			if (auto str = dynamic_cast<string_object_raw_pointer>(val);str)
			{
				return std::format("{0}{1}", type_name_of<string_object_raw_pointer>(), str->string());
			}

			return std::format("{0}{1:<0x}", type_name_of<std::decay_t<decltype(val)>>(), (uintptr_t)val);
		}
		else if constexpr(std::is_same_v<variable_name_type, std::decay_t<T>>)
		{
			return std::format("{}{}", type_name_of<std::decay_t<decltype(val)>>(), val);
		}
		else
		{
			return std::format("{}{}", type_name_of<std::decay_t<decltype(val)>>(), std::to_string(val));
		}
	}

	template<>
	std::string operator()(nil_value_type val);


private:
	struct type_names
	{
		template<typename T>
		struct type_name
		{
			static constexpr std::string_view value{ "<Unknown>" };
		};

		template<>
		struct type_name<integer_value_type>
		{
			static constexpr std::string_view value{ "<integer>" };
		};

		template<>
		struct type_name<floating_value_type>
		{
			static constexpr std::string_view value{ "<floating>" };
		};

		template<>
		struct type_name<object_value_type>
		{
			static constexpr std::string_view value{ "<object>" };
		};

		template<>
		struct type_name<string_object_raw_pointer>
		{
			static constexpr std::string_view value{ "<string>" };
		};

		template<>
		struct type_name<boolean_value_type>
		{
			static constexpr std::string_view value{ "<boolean>" };
		};

		template<>
		struct type_name<nil_value_type>
		{
			static constexpr std::string_view value{ "<nil type>" };
		};

		template<>
		struct type_name<variable_name_type>
		{
			static constexpr std::string_view value{ "<variable name>" };
		};
	};

	template<typename T>
	std::string type_name_of()
	{
		if (!show_type_)
		{
			return "";
		}

		return std::string{ type_names::type_name<T>::value };
	}

	bool show_type_{ false };
};

//using value = std::variant<
//		scanning::integer_literal_type,
//		scanning::floating_literal_type,
//		scanning::boolean_literal_type,
//		scanning::string_literal_type,
//		scanning::nil_value_tag_type,
//		std::shared_ptr<class callable>,
//		std::shared_ptr<class lox_instance>,
//		std::shared_ptr<class lox_initializer_list>>;
}

#include <string>
#include <format>

namespace std
{
template<>
struct std::formatter<clox::interpreting::vm::value> : std::formatter<std::string>
{
	auto format(const clox::interpreting::vm::value& val, format_context& ctx)
	{
		return formatter<string>::format(
				std::visit(clox::interpreting::vm::value_stringify_visitor{ true }, val), ctx);
	}
};
}

#ifdef V
#error "V is already defined"
#undef V
#else
#define V(op) (op_code_value((op)))
#define VC(sec, main) (compose_opcode((sec),(main)))

#endif

