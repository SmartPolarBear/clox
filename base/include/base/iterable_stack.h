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
// Created by cleve on 10/1/2021.
//

#pragma once

#include <concepts>
#include <vector>
#include <stack>

namespace clox::base
{

template<typename T, typename TVal>
concept IterableContainer=
requires(T c, TVal v)
{
	c.begin();
	c.end();
	c.cbegin();
	c.cend();
	c.rbegin();
	c.rend();
	c.crbegin();
	c.crend();

	{ c.back() }->std::convertible_to<TVal&>;
	{ c.push_back(v) };
	{ c.pop_back() };
};

template<typename T, IterableContainer<T> Container=std::vector<T>>
class iterable_stack final
		: public std::stack<T, Container>
{
private:
	using base_type = std::stack<T, Container>;
public:
	using iterator = typename Container::iterator;
	using const_iterator = typename Container::const_iterator;
	using reverse_iterator = typename Container::reverse_iterator;
	using const_reverse_iterator = typename Container::const_reverse_iterator;

	typename base_type::reference top_n(typename base_type::size_type n)
	{
		return *(rbegin() + n);
	}

	constexpr iterator begin() noexcept
	{
		return this->c.begin();
	}

	constexpr iterator end() noexcept
	{
		return this->c.end();
	}

	constexpr const_iterator cbegin() noexcept
	{
		return this->c.cbegin();
	}

	constexpr const_iterator cend() noexcept
	{
		return this->c.cend();
	}

	constexpr reverse_iterator rbegin() noexcept
	{
		return this->c.rbegin();
	}

	constexpr reverse_iterator rend() noexcept
	{
		return this->c.rend();
	}

	constexpr const_reverse_iterator crbegin() noexcept
	{
		return this->c.crbegin();
	}

	constexpr const_reverse_iterator crend() noexcept
	{
		return this->c.crend();
	}

};

}

