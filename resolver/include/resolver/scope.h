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
// Created by cleve on 8/15/2021.
//

#pragma once

#include <parser/gen/parser_classes.inc>

#include <interpreter/classic/interpreter.h>

#include <resolver/lox_type.h>
#include <resolver/callable_type.h>
#include <resolver/class_type.h>
#include <resolver/instance_type.h>

#include <resolver/function.h>
#include <resolver/upvalue.h>

#include <vector>
#include <stack>
#include <map>
#include <unordered_map>
#include <memory>
#include <tuple>
#include <thread>
#include <variant>

namespace clox::resolving
{

struct global_scope_tag_type
{
};

static constexpr global_scope_tag_type global_scope_tag{};

enum class scope_types
{
	SCOPE = 1,
	FUNCTION_SCOPE,
	CLASS_BASE_SCOPE,
	CLASS_FIELD_SCOPE,
};

class scope
		: public std::enable_shared_from_this<scope>
{
public:
	friend class scope_iterator;

	friend class resolver;

	using name_table_type = std::unordered_map<std::string, std::shared_ptr<symbol>>;
	using type_table_type = std::unordered_map<std::string, std::shared_ptr<lox_type>>;
	using scope_list_type = std::vector<std::shared_ptr<scope>>;

	friend class resolver;

	scope() = delete;

	explicit scope(const std::shared_ptr<scope>& parent)
			: parent_(parent), container_func_(parent->container_func_)
	{
	}

	explicit scope(const std::shared_ptr<scope>& parent, global_scope_tag_type)
			: parent_(parent), container_func_(parent->container_func_), is_global_(true)
	{
	}

	explicit scope(const std::shared_ptr<scope>& parent, function_id_type id)
			: parent_(parent), container_func_(id)
	{
	}

	explicit scope(const std::shared_ptr<scope>& parent, function_id_type id, global_scope_tag_type)
			: parent_(parent), container_func_(id), is_global_(true)
	{
	}

	[[nodiscard]] virtual scope_types scope_type() noexcept
	{
		return scope_types::SCOPE;
	}

	[[nodiscard]] bool contains_name(const std::string& name) const
	{
		return names_.contains(name);
	}

	[[nodiscard]] bool contains_type(const std::string& name) const
	{
		return types_.contains(name);
	}

	[[nodiscard]] type_table_type::value_type::second_type type(const std::string& name) const
	{
		return types_.at(name);
	}

	template<std::derived_from<lox_type> T>
	[[nodiscard]] std::shared_ptr<T> type_typed(const std::string& n) const
	{
		return std::static_pointer_cast<T>(type(n));
	}


	[[nodiscard]] name_table_type& names()
	{
		return names_;
	}

	[[nodiscard]] name_table_type::value_type::second_type name(const std::string& name) const
	{
		return names_.at(name);
	}

	template<std::derived_from<symbol> T>
	[[nodiscard]] std::shared_ptr<T> name_typed(const std::string& n) const
	{
		return downcast_symbol<T>(name(n));
	}

	template<std::derived_from<symbol> T>
	std::shared_ptr<T> find_name(const std::string n)
	{
		for (auto p = this->shared_from_this(); p; p = p->parent_.lock())
		{
			if (p->contains_name(n))
			{
				return p->name_typed<T>(n);
			}
		}
		return nullptr;
	}


	[[nodiscard]]type_table_type& types()
	{
		return types_;
	}

	[[nodiscard]]function_id_type container_function() const
	{
		return container_func_;
	}

	[[nodiscard]] bool is_global() const
	{
		return is_global_;
	}


private:
	[[nodiscard]] scope_list_type::iterator& last() const
	{
		if (!last_.has_value())
		{
			last_ = children_.begin();
		}

		return last_.value();
	}

	mutable function_id_type container_func_{};

	mutable name_table_type names_{};

	mutable type_table_type types_{};

	mutable size_t visit_count_{ 0 };

	mutable bool is_global_{ false };

	mutable scope_list_type children_{};

	mutable std::optional<scope_list_type::iterator> last_{};

	mutable std::weak_ptr<scope> parent_{};

};

class function_scope
		: public scope
{
public:
	friend class scope_iterator;

	friend class resolver;

	using upvalue_list_type = std::vector<std::shared_ptr<upvalue>>;

public:
	explicit function_scope(const std::shared_ptr<scope>& parent, const std::shared_ptr<function_scope>& fparent,
			function_id_type id)
			: scope(parent, id), parent_function_(fparent)
	{
	}

	explicit function_scope(const std::shared_ptr<scope>& parent, const std::shared_ptr<function_scope>& fparent,
			function_id_type id, global_scope_tag_type)
			: scope(parent, id, global_scope_tag), parent_function_(fparent)
	{
	}

	[[nodiscard]] scope_types scope_type() noexcept override
	{
		return scope_types::FUNCTION_SCOPE;
	}

	std::shared_ptr<upvalue> put_upvalue(const std::shared_ptr<upvalue>& upvalue)
	{
		for (const auto& item: upvalues_)
		{
			if (*item == *upvalue)
			{
				return item;
			}
		}

		upvalue->index_ = upvalues_.size();
		upvalues_.push_back(upvalue);

		return upvalue;
	}

	upvalue_list_type& upvalues()
	{
		return upvalues_;
	}

private:

	[[nodiscard]] scope_list_type::iterator& last_function() const
	{
		if (!last_function_.has_value())
		{
			last_function_ = child_functions_.begin();
		}

		return last_function_.value();
	}

	upvalue_list_type upvalues_{};

	mutable scope_list_type child_functions_{};

	mutable std::weak_ptr<function_scope> parent_function_{};

	mutable std::optional<scope_list_type::iterator> last_function_{};
};

class class_base_scope
		: public scope
{

};

class class_field_scope
		: public scope
{

};

}
