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
#include <helper/enum.h>

#include "type/callable_type.h"

#include <cassert>
#include <tuple>
#include <utility>
#include <format>
#include <stdexcept>

using namespace clox;

using namespace clox::helper;

using namespace clox::resolving;


redefined_symbol::redefined_symbol(const std::shared_ptr<lox_callable_type>& cur,
		const std::shared_ptr<lox_callable_type>& conflict)
		: cur_(cur),
		  conflict_(),
		  std::runtime_error(std::format("Ambiguous function signature: {} and {}",
				  cur->printable_string(), conflict->printable_string()))
{

}

too_many_params::too_many_params(const std::shared_ptr<lox_callable_type>& func)
		: func_(func),
		  std::runtime_error(std::format("Too many parameters in function signature {}", func->printable_string()))
{

}


lox_callable_type::lox_callable_type(const std::string& name, return_type_variant return_type,
		param_list_type params, bool ctor, bool native)
		: name_(name),
		  return_type_(std::move(return_type)),
		  params_(std::move(params)),
		  lox_object_type(name, TYPE_ID_FUNC, TYPE_CLASS | FLAG_CALLABLE, object())
{
	if (ctor)
	{
		lox_object_type::flags_ |= FLAG_CTOR;
	}

	if (native)
	{
		lox_object_type::flags_ |= FLAG_NATIVE;
	}
}

lox_callable_type::lox_callable_type(lox_callable_type::return_type_variant return_type,
		lox_callable_type::param_list_type params, bool ctor, bool native)
		: name_(),
		  return_type_(std::move(return_type)),
		  params_(std::move(params)),
		  lox_object_type("", TYPE_ID_FUNC, TYPE_CLASS | FLAG_CALLABLE, object())
{
	if (ctor)
	{
		lox_object_type::flags_ |= FLAG_CTOR;
	}

	if (native)
	{
		lox_object_type::flags_ |= FLAG_NATIVE;
	}
}


std::string lox_callable_type::printable_string()
{
	auto type_string = return_type_deduced() ? get<std::shared_ptr<lox_type>>(return_type_)->printable_string()
											 : "<deducing deferred>";

	auto ret = std::format("<callable object {} -> {} ( ", name_, type_string);

	for (const auto& param: params_)
	{
		if (param.first.has_value())
		{
			ret += std::format("{}:{},", param.first.value().lexeme(), param.second->printable_string());
		}
		else
		{
			ret += std::format("{},", param.second->printable_string());
		}
	}

	*ret.rbegin() = ')';
	return ret;
}

bool lox_callable_type::operator<(const lox_type& target) const
{
	return false;
}

bool lox_callable_type::operator==(const lox_type& another) const
{
	if (!is_callable(another))return false;

	const auto& callable = dynamic_cast<const lox_callable_type&>(another);

	if (name_ != callable.name_)return false;

	if (params_.size() != callable.params_.size())return false;

	auto param_size = params_.size();
	for (decltype(param_size) i = 0; i < param_size; i++)
	{
		if (params_[i].second != callable.params_[i].second)
		{
			return false;
		}
	}

	return true;
}

bool lox_callable_type::operator!=(const lox_type& another) const
{
	return !(*this == another);
}

std::shared_ptr<lox_type> lox_callable_type::return_type() const
{
	if (!return_type_deduced())
	{
		throw std::logic_error{ "deferred return type deducing isn't run in time." };
	}

	return get<std::shared_ptr<lox_type>>(return_type_);
}

void lox_callable_type::set_return_type(const std::shared_ptr<lox_type>& return_type)
{
	if (return_type_deduced())
	{
		throw std::logic_error{ "deferred return type deducing is run twice." };
	}

	return_type_ = return_type;
}

bool lox_callable_type::return_type_deduced() const
{
	return !std::holds_alternative<type_deduce_defer_tag>(return_type_);
}


void lox_overloaded_metatype::put(const std::shared_ptr<parsing::statement>& stmt,
		const std::shared_ptr<lox_callable_type>& callable)
{
	auto param_list = callable->params();
	auto node = root_;

	for (auto param_iter = param_list.begin();
		 node && node->depth_ <= lox_callable_type::MAX_PARAMETER_COUNT && param_iter != param_list.end();
		 param_iter++)
	{
		if (node->depth_ > lox_callable_type::MAX_PARAMETER_COUNT)
		{
			throw too_many_params{ callable };
		}

		if (!node->next_.contains(param_iter->second)) // exact match
		{
			auto next = std::make_shared<lox_overloaded_node>();
			node->next_.insert(std::make_pair(param_iter->second, next));
			next->parent(node);
		}

		node = node->next_[param_iter->second];
	}

	if (node->end_)
	{
		throw redefined_symbol{ callable, node->callable_ };
	}

	node->end_ = true;
	node->stmt_ = stmt;
	node->callable_ = callable;

	all_.emplace_back(stmt, callable);
	last_ = callable;
}

std::optional<std::tuple<std::shared_ptr<parsing::statement>, std::shared_ptr<lox_callable_type>>>
lox_overloaded_metatype::get(const std::vector<std::shared_ptr<lox_type>>& params)
{
	auto node = overloading_resolve(
			const_cast<std::vector<std::shared_ptr<lox_type>>&>(params).begin(),
			const_cast<std::vector<std::shared_ptr<lox_type>>&>(params).end(),
			root_);

	if (!node)
	{
		return std::nullopt;
	}

	return std::make_tuple(node->stmt_, node->callable_);
}

std::shared_ptr<lox_overloaded_metatype::lox_overloaded_node>
lox_overloaded_metatype::overloading_resolve(std::vector<std::shared_ptr<lox_type>>::iterator param_iter,
		const std::vector<std::shared_ptr<lox_type>>::iterator end,
		std::shared_ptr<lox_overloaded_node> node)
{
	if (param_iter == end)
	{
		if (node->end_)
		{
			return node;
		}
		else
		{
			return nullptr;
		}
	}

	type_id_diff diff = INT64_MAX;
	std::shared_ptr<lox_overloaded_node> next{ nullptr };

	for (auto& iter: node->next_)
	{
		if (lox_type::unify(*iter.first, **param_iter))
		{
			auto base_object = std::static_pointer_cast<lox_object_type>(iter.first);
			auto derive_object = std::static_pointer_cast<lox_object_type>(*param_iter);
			if (auto d = derive_object->id() - base_object->id();d <= diff)
			{
				auto next_node = overloading_resolve(param_iter + 1, end, iter.second);
				if (next_node)
				{
					next = next_node;
					diff = d;
				}
			}
		}
	}

	return next;
}
