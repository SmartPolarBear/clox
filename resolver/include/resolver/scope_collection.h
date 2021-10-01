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

#include <resolver/scope.h>

namespace clox::resolving
{


class scope_iterator
{
public:

	friend class scope_collection;

	scope_iterator() = delete;

	explicit scope_iterator(std::shared_ptr<scope> s) : data_(std::move(s))
	{
	}

	~scope_iterator() = default;

	scope_iterator(scope_iterator&&) = default;

	scope_iterator(const scope_iterator&) = default;

	scope_iterator& operator=(const scope_iterator&) = default;

	std::shared_ptr<scope> operator*();

	scope_iterator& operator++();

	scope_iterator operator++(int);

	scope_iterator& operator--();

	scope_iterator operator--(int);

	bool operator==(const scope_iterator& another) const;

private:
	std::shared_ptr<scope> data_;

};

class scope_collection final
{
public:
	using iterator = scope_iterator;

	explicit scope_collection(std::shared_ptr<scope> root) : root_(std::move(root))
	{
	}

	iterator begin();

	iterator end();

private:
	std::shared_ptr<scope> root_{};
};


}