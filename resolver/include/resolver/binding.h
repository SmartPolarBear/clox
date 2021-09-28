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
// Created by cleve on 8/19/2021.
//

#pragma once

#include <parser/gen/parser_classes.inc>

#include <resolver/lox_type.h>
#include <resolver/function.h>

#include <memory>
#include <optional>

namespace clox::resolving
{

enum class binding_type
{
	BINDING_VARIABLE,
	BINDING_FUNCTION,
	BINDING_OPERATOR,
};

template<typename T>
struct binding_tag;

class binding
{
public:
	[[nodiscard]] virtual std::shared_ptr<parsing::expression> expression() const = 0;

	[[nodiscard]] virtual binding_type type() const = 0;
};

class variable_binding
		: public binding
{
public:
	variable_binding() = default;

	explicit variable_binding(std::shared_ptr<parsing::expression> e, int64_t d)
			: expr_(std::move(e)), depth_(d)
	{
	}

	[[nodiscard]] binding_type type() const override
	{
		return binding_type::BINDING_VARIABLE;
	}

	[[nodiscard]] std::shared_ptr<parsing::expression> expression() const override
	{
		return expr_;
	}

	[[nodiscard]] int64_t depth() const
	{
		return depth_;
	}

private:
	std::shared_ptr<parsing::expression> expr_{ nullptr };
	int64_t depth_{ 0 };
};


template<>
struct binding_tag<variable_binding>
{
	static constexpr binding_type type = binding_type::BINDING_VARIABLE;
};

class function_binding
		: public binding
{
public:
	[[nodiscard]] binding_type type() const override
	{
		return binding_type::BINDING_FUNCTION;
	}

	function_binding() = default;

	explicit function_binding(std::shared_ptr<parsing::call_expression> e, std::shared_ptr<parsing::statement> s,
			function_id_type id)
			: expr_(std::move(e)), stmt_(std::move(s)), id_(id)
	{
	}

	[[nodiscard]] std::shared_ptr<parsing::expression> expression() const override
	{
		return expr_;
	}

	[[nodiscard]] std::shared_ptr<parsing::statement> statement() const
	{
		return stmt_;
	}

	[[nodiscard]] function_id_type id() const
	{
		return id_;
	}

private:
	std::shared_ptr<parsing::call_expression> expr_{ nullptr };

	std::shared_ptr<parsing::statement> stmt_{ nullptr };

	function_id_type id_{ FUNCTION_ID_INVALID };
};

template<>
struct binding_tag<function_binding>
{
	static constexpr binding_type type = binding_type::BINDING_FUNCTION;
};


class operator_binding
		: public binding
{
public:
	[[nodiscard]] binding_type type() const override
	{
		return binding_type::BINDING_OPERATOR;
	}

	operator_binding() = default;

	explicit operator_binding(std::shared_ptr<parsing::expression> e, std::shared_ptr<parsing::call_expression> ce)
			: expr_(std::move(e)), call_expr_(std::move(ce))
	{
	}

	[[nodiscard]] std::shared_ptr<parsing::expression> expression() const override
	{
		return expr_;
	}

	[[nodiscard]] std::shared_ptr<parsing::call_expression> operator_implementation_call() const
	{
		return call_expr_;
	}

private:
	std::shared_ptr<parsing::expression> expr_{ nullptr };
	std::shared_ptr<parsing::call_expression> call_expr_{ nullptr };
};

template<>
struct binding_tag<operator_binding>
{
	static constexpr binding_type type = binding_type::BINDING_OPERATOR;
};

template<typename T>
[[maybe_unused, nodiscard]] static inline std::optional<std::shared_ptr<T>>
downcast_binding(const std::shared_ptr<binding>& binding)
{
	if (binding->type() == binding_tag<T>::type) [[likely]]
	{
		return std::static_pointer_cast<T>(binding);
	}
	else [[unlikely]]
	{
		return std::nullopt;
	}
}

class binding_table
{
public:
	binding_table() = default;

	template<typename TBinding, class... Args>
	void put(const std::shared_ptr<parsing::expression>& expr, Args&& ... args)
	{
		auto binding = std::make_shared<TBinding>(std::forward<Args>(args)...);
		bindings_[expr] = binding;
	}

	[[nodiscard]] bool contains(const std::shared_ptr<parsing::expression>& e)
	{
		return bindings_.contains(e);
	}

	std::optional<std::shared_ptr<binding>> get(const std::shared_ptr<parsing::expression>& e);

	template<typename T>
	std::optional<std::shared_ptr<T>> get_typed(const std::shared_ptr<parsing::expression>& e)
	{
		auto binding = get(e);
		if (binding.has_value()) [[likely]]
		{
			return downcast_binding<T>(binding.value());
		}

		return binding;
	}

private:
	std::unordered_map<std::shared_ptr<parsing::expression>, std::shared_ptr<binding>> bindings_{};
};


}
