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
// Created by cleve on 8/8/2021.
//

#pragma once

#include <resolver/lox_type.h>
#include <resolver/object_type.h>

#include <resolver/function.h>

#include <parser/parser.h>

#include <vector>
#include <variant>
#include <map>
#include <optional>
#include <stdexcept>

namespace std
{
template<>
struct hash<std::shared_ptr<clox::resolving::lox_type>>
{
	auto operator()(const std::shared_ptr<clox::resolving::lox_type>& p) const noexcept
	{
		return p->id();
	}
};
}

namespace clox::resolving
{

class lox_callable_type
	: public lox_object_type,
	  public std::enable_shared_from_this<lox_callable_type>
{
 public:
	using parameter_type = std::pair<std::optional<scanning::token>, std::shared_ptr<lox_type>>;
	using param_list_type = std::vector<parameter_type>;
	using return_type_variant = std::variant<type_deduce_defer_tag, std::shared_ptr<lox_type>>;

	static inline constexpr size_t MAX_PARAMETER_COUNT = 256;
 public:

	static inline constexpr param_list_type empty_parameter_list()
	{
		return param_list_type{};
	}

	template<typename... TArgs>
	static inline param_list_type parameter_list_of(TArgs&& ... args)
	{
		return lox_callable_type::param_list_type{
			make_pair(std::nullopt, std::forward<TArgs>(args))...
		};
	}

	[[nodiscard]] explicit lox_callable_type(return_type_variant return_type,
		param_list_type params, bool ctor = false, bool native = false);

	[[nodiscard]] lox_callable_type(const std::string& name, return_type_variant return_type,
		param_list_type params, bool ctor = false, bool native = false);

	std::string printable_string() override;

	[[nodiscard]] bool is_ctor() const
	{
		return lox_object_type::flags() & FLAG_CTOR;
	}

	bool operator<(const lox_type& target) const override;

	bool operator==(const lox_type& lox_type) const override;

	bool operator!=(const lox_type& lox_type) const override;

	auto param_size() const
	{
		return params_.size();
	}

	std::shared_ptr<lox_type> param_type(size_t i) const
	{
		return params_.at(i).second;
	}

	param_list_type params() const
	{
		return params_;
	}

	[[nodiscard]] bool return_type_deduced() const;

	void set_return_type(const std::shared_ptr<lox_type>&);

	std::shared_ptr<lox_type> return_type() const;

 private:
	std::string name_{};

	return_type_variant return_type_{};

	param_list_type params_{};

};

class lox_native_callable_type final
	: public lox_callable_type
{
 public:
	[[nodiscard]] explicit lox_native_callable_type(return_type_variant return_type,
		param_list_type params, bool ctor = false);
};

class lox_overloaded_metatype final
	: public lox_object_type,
	  public std::enable_shared_from_this<lox_overloaded_metatype>
{
 public:
	explicit lox_overloaded_metatype(const std::string& name) :
		lox_object_type(name, TYPE_ID_OVERLOADED_FUNC, FLAG_CALLABLE, nullptr),
		root_(std::make_shared<lox_overloaded_node>())
	{
	}

	~lox_overloaded_metatype() = default;

	lox_overloaded_metatype(const lox_overloaded_metatype&) = default;

	lox_overloaded_metatype& operator=(const lox_overloaded_metatype&) = default;

	auto begin() const
	{
		return all_.begin();
	}

	auto end() const
	{
		return all_.end();
	}

	std::shared_ptr<lox_callable_type> last() const
	{
		return last_;
	}

	void put(const std::shared_ptr<parsing::statement>& stmt,
		const std::shared_ptr<lox_callable_type>& callable);

	std::optional<std::tuple<std::shared_ptr<parsing::statement>, std::shared_ptr<lox_callable_type>>>
	get(const std::vector<std::shared_ptr<lox_type>>& params);

 private:

	class lox_overloaded_node final
		: public std::enable_shared_from_this<lox_overloaded_node>
	{
	 public:
		friend class lox_overloaded_metatype;

		lox_overloaded_node() = default;

		~lox_overloaded_node() = default;

		[[nodiscard]] explicit lox_overloaded_node(std::shared_ptr<parsing::statement> stmt,
			std::shared_ptr<lox_callable_type> callable)
			: end_(true), stmt_(std::move(stmt)), callable_(std::move(callable))
		{
		}

		void parent(const std::shared_ptr<lox_overloaded_node>& pa)
		{
			parent_ = pa;
			depth_ = pa->depth_ + 1;
		}

	 private:

		size_t depth_{ 0 };
		std::weak_ptr<lox_overloaded_node> parent_;

		bool end_{};
		std::shared_ptr<parsing::statement> stmt_;
		std::shared_ptr<lox_callable_type> callable_;

		std::unordered_map<std::shared_ptr<lox_type>, std::shared_ptr<lox_overloaded_node>> next_{};
	};

	std::shared_ptr<lox_overloaded_metatype::lox_overloaded_node>
	overloading_resolve(std::vector<std::shared_ptr<lox_type>>::iterator param_iter,
		std::vector<std::shared_ptr<lox_type>>::iterator end, std::shared_ptr<lox_overloaded_node> node);

	std::shared_ptr<lox_overloaded_node> root_{};

	std::shared_ptr<lox_callable_type> last_{};

	std::vector<std::pair<std::shared_ptr<parsing::statement>, std::shared_ptr<lox_callable_type>>> all_{};
};

class redefined_symbol final
	: public std::runtime_error
{
 public:
	explicit redefined_symbol(const std::shared_ptr<lox_callable_type>& cur,
		const std::shared_ptr<lox_callable_type>& conflict);

 private:
	std::shared_ptr<lox_callable_type> cur_{};
	std::shared_ptr<lox_callable_type> conflict_{};

};

class too_many_params final
	: public std::runtime_error
{
 public:
	explicit too_many_params(const std::shared_ptr<lox_callable_type>& func);

 private:
	std::shared_ptr<lox_callable_type> func_{};

};

}
