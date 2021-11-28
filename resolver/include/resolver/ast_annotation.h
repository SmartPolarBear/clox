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
// Created by cleve on 11/9/2021.
//

#pragma once

#include <parser/expression.h>
#include <parser/statement.h>

#include <resolver/lox_type.h>
#include <resolver/class_type.h>
#include <resolver/function.h>
#include <resolver/symbol.h>

#include <vector>

namespace clox::resolving
{


template<typename T>
struct annotation_tag;


class variable_annotation final
		: public parsing::ast_annotation
{
public:
	variable_annotation() = default;

	explicit variable_annotation(int64_t d, std::shared_ptr<named_symbol> symbol,
			std::shared_ptr<clox::resolving::upvalue> upval = nullptr) :
			depth_(d), symbol_(std::move(symbol)), upvalue_(std::move(upval))
	{
	}

	[[nodiscard]] parsing::ast_annotation_type type() const override
	{
		return parsing::ast_annotation_type::AST_ANNOTATION_VARIABLE;
	}


	[[nodiscard]] int64_t depth() const
	{
		return depth_;
	}

	[[nodiscard]] std::shared_ptr<named_symbol> symbol() const
	{
		return symbol_;
	}

	[[nodiscard]] std::shared_ptr<clox::resolving::upvalue> upvalue() const
	{
		return upvalue_;
	}

private:
	int64_t depth_{ 0 };

	std::shared_ptr<named_symbol> symbol_{};

	std::shared_ptr<clox::resolving::upvalue> upvalue_{};
};


template<>
struct annotation_tag<variable_annotation>
{
	static constexpr parsing::ast_annotation_type type = parsing::ast_annotation_type::AST_ANNOTATION_VARIABLE;
};

class function_annotation final
		: public parsing::ast_annotation
{
public:
	enum function_binding_flags : uint32_t
	{
		FB_CTOR = 1 << 1,
		FB_METHOD = 1 << 2,
		FB_BASE = 1 << 3,
	};

	[[nodiscard]] parsing::ast_annotation_type type() const override
	{
		return parsing::ast_annotation_type::AST_ANNOTATION_FUNCTION;
	}

	function_annotation() = default;

	explicit function_annotation(std::shared_ptr<parsing::statement> s,
			function_id_type id, uint32_t flags)
			: stmt_(std::move(s)), id_(id), flags_(flags)
	{
	}

	explicit function_annotation(std::shared_ptr<parsing::statement> s,
			function_id_type id, uint32_t flags, std::shared_ptr<resolving::lox_class_type> cls)
			: stmt_(std::move(s)), id_(id), flags_(flags), class_(std::move(cls))
	{
	}

	[[nodiscard]] std::shared_ptr<parsing::statement> statement() const
	{
		return stmt_;
	}

	[[nodiscard]] function_id_type id() const
	{
		return id_;
	}

	[[nodiscard]] bool is_ctor() const
	{
		return flags_ & FB_CTOR;
	}

	[[nodiscard]] bool is_method() const
	{
		return flags_ & FB_METHOD;
	}

	[[nodiscard]] std::shared_ptr<resolving::lox_class_type> ctor_class_type()
	{
		return class_;
	}

private:

	std::shared_ptr<parsing::statement> stmt_{ nullptr };

	std::shared_ptr<resolving::lox_class_type> class_{ nullptr };

	function_id_type id_{ FUNCTION_ID_INVALID };

	uint32_t flags_{ 0 };
};

template<>
struct annotation_tag<function_annotation>
{
	static constexpr parsing::ast_annotation_type type = parsing::ast_annotation_type::AST_ANNOTATION_FUNCTION;
};


class operator_annotation final
		: public parsing::ast_annotation
{
public:
	[[nodiscard]] parsing::ast_annotation_type type() const override
	{
		return parsing::ast_annotation_type::AST_ANNOTATION_OPERATOR;
	}

	operator_annotation() = default;

	explicit operator_annotation(std::shared_ptr<parsing::call_expression> ce)
			: call_expr_(std::move(ce))
	{
	}


	[[nodiscard]] std::shared_ptr<parsing::call_expression> operator_implementation_call() const
	{
		return call_expr_;
	}

private:
	std::shared_ptr<parsing::call_expression> call_expr_{ nullptr };
};

template<>
struct annotation_tag<operator_annotation>
{
	static constexpr parsing::ast_annotation_type type = parsing::ast_annotation_type::AST_ANNOTATION_OPERATOR;
};


class class_annotation final
		: public parsing::ast_annotation
{
public:
	explicit class_annotation(std::shared_ptr<class lox_class_type> ct)
			: class_type_(std::move(ct))
	{
	}

	[[nodiscard]] std::shared_ptr<class lox_class_type> class_type() const
	{
		return class_type_;
	}

	[[nodiscard]] parsing::ast_annotation_type type() const override
	{
		return parsing::ast_annotation_type::AST_ANNOTATION_CLASS;
	}

	void set_as_method(const std::shared_ptr<parsing::call_expression>& ce)
	{
		method_ = true;
		caller_ = ce;
	}

	[[nodiscard]] bool is_method() const
	{
		return method_;
	}


	[[nodiscard]] std::shared_ptr<parsing::call_expression> method_caller() const
	{
		return caller_;
	}

private:
	std::shared_ptr<class lox_class_type> class_type_{};

	bool method_{ false };
	std::shared_ptr<parsing::call_expression> caller_{ nullptr };
};

template<>
struct annotation_tag<class_annotation>
{
	static constexpr parsing::ast_annotation_type type = parsing::ast_annotation_type::AST_ANNOTATION_CLASS;
};

class base_annotation final
		: public parsing::ast_annotation
{
public:
	enum class base_field_type
	{
		FIELD, METHOD
	};

	explicit base_annotation(int32_t base_index, base_field_type type,
			int32_t field_id)
			: base_index_(base_index), field_type_(type), field_id_(field_id)
	{
	}


	[[nodiscard]] parsing::ast_annotation_type type() const override
	{
		return parsing::ast_annotation_type::AST_ANNOTATION_BASE;
	}

	[[nodiscard]] int32_t index() const
	{
		return base_index_;
	}

	[[nodiscard]] int32_t field_id() const
	{
		assert(field_id_ != -1);
		return field_id_;
	}

	[[nodiscard]] auto field_type() const
	{
		return field_type_;
	}

	void set_field_id(int32_t id)
	{
		field_id_ = id;
	}

private:
	int32_t base_index_{ -1 };

	base_field_type field_type_{};
	int32_t field_id_{ -1 };
};

template<>
struct annotation_tag<base_annotation>
{
	static constexpr parsing::ast_annotation_type type = parsing::ast_annotation_type::AST_ANNOTATION_BASE;
};

template<typename T>
[[maybe_unused, nodiscard]] static inline std::shared_ptr<T>
downcast_annotation(const std::shared_ptr<parsing::ast_annotation>& anno)
{
	if (anno->type() == annotation_tag<T>::type) [[likely]]
	{
		return std::static_pointer_cast<T>(anno);
	}
	else [[unlikely]]
	{
		return nullptr;
	}
}


}