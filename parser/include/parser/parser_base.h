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
// Created by cleve on 6/18/2021.
//
#pragma once

#include <concepts>

#include <parser/gen/parser_classes.inc>

namespace clox::parsing
{

class parser_class_base
{
public:
	bool has_parent_node() const
	{
		return !parent_node_.expired();
	}

	[[nodiscard]] std::shared_ptr<parser_class_base> parent_node() const
	{
		return parent_node_.lock();
	}

	void set_parent_node(const std::shared_ptr<parser_class_base>& pa)
	{
		parent_node_ = std::weak_ptr<parser_class_base>{ pa };
	}

	[[nodiscard]] virtual parser_class_type get_type() const
	{
		return parser_class_type::PC_TYPE_invalid;
	};

protected:
	std::weak_ptr<parser_class_base> parent_node_{};
};

template<typename T, typename TChild>
requires (!std::ranges::range<TChild>)
void __single_set_parent(std::shared_ptr<T> parent, TChild child)
{
	if (child)
		child->set_parent_node(parent);
}

template<typename T, typename TChildren>
requires std::ranges::range<TChildren>
void __single_set_parent(std::shared_ptr<T> parent, TChildren children)
{
	for (auto child: children)
	{
		if (child)child->set_parent_node(parent);
	}
}

template<typename T, typename ...TChildren>
std::shared_ptr<T> set_parent(std::shared_ptr<T> parent, TChildren... children)
{
	(__single_set_parent(parent, children), ...);
	return parent;
}

}
