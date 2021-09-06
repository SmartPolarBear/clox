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

#include <variant>
#include <string>

#include <memory>
#include <map>


namespace clox::interpreting::vm
{
using value = std::variant<scanning::integer_literal_type,
		scanning::floating_literal_type,
		scanning::boolean_literal_type,
		scanning::string_literal_type,
		scanning::nil_value_tag_type>;

/// Get number from value, promoting to floating type
/// \param val the value variant
/// \return numeric value promoted to floating type
/// \throws invalid_value
scanning::floating_literal_type get_number_promoted(const value& val);

class value_stringify_visitor
{
public:
	value_stringify_visitor() = default;

	explicit value_stringify_visitor(bool show_type);

	template<typename T>
	std::string operator()(T val)
	{
		if constexpr(std::is_same_v<std::string, std::decay_t<T>>) // To avoid copying strings
		{
			return std::format("{} {}", type_name_of<std::decay_t<decltype(val)>>(), val);
		}
		else
		{
			return std::format("{} {}", type_name_of<std::decay_t<decltype(val)>>(), std::to_string(val));
		}
	}

	template<>
	std::string operator()(scanning::nil_value_tag_type val);


private:
	struct type_names
	{
		template<typename T>
		struct type_name
		{
			static constexpr std::string_view value{ "<Unknown>" };
		};

		template<>
		struct type_name<scanning::integer_literal_type>
		{
			static constexpr std::string_view value{ "<integer>" };
		};

		template<>
		struct type_name<scanning::floating_literal_type>
		{
			static constexpr std::string_view value{ "<floating>" };
		};

		template<>
		struct type_name<scanning::string_literal_type>
		{
			static constexpr std::string_view value{ "<string>" };
		};

		template<>
		struct type_name<scanning::boolean_literal_type>
		{
			static constexpr std::string_view value{ "<boolean>" };
		};

		template<>
		struct type_name<scanning::nil_value_tag_type>
		{
			static constexpr std::string_view value{ "<nil type>" };
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
#define V(op) op_code_value((op))
#endif

