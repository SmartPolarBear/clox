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

#include <resolver/scope_collection.h>

using namespace clox::resolving;

std::shared_ptr<scope> clox::resolving::scope_iterator::operator*()
{
	return data_;
}

scope_iterator& scope_iterator::operator++()
{
	if (data_->last_ == data_->children_.end())
	{
		data_ = nullptr;
	}
	else
	{
		data_ = *(data_->last_++);
		data_->visit_count_++;
	}

	return *this;
}

scope_iterator scope_iterator::operator++(int)
{
	auto old = *this;
	operator++();
	return old;
}

scope_iterator& scope_iterator::operator--()
{
	if (auto pa = data_->parent_.lock();pa)
	{
		data_ = pa;
		data_->visit_count_++;
	}
	else
	{
		this->data_ = nullptr;
	}

	return *this;
}

scope_iterator scope_iterator::operator--(int)
{
	auto old = *this;
	operator--();
	return old;
}

bool scope_iterator::operator==(const scope_iterator& another) const
{
	return data_ == another.data_;
}

scope_collection::iterator scope_collection::begin()
{
	return scope_iterator{ root_ };
}

scope_collection::iterator scope_collection::end()
{
	return scope_iterator{ nullptr };
}
