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
// Created by cleve on 9/6/2021.
//

#include <interpreter/vm/string_object.h>
#include "interpreter/vm/garbage_collector.h"

#include <string>

using namespace std;
using namespace clox::interpreting::vm;

clox::interpreting::vm::string_object::string_interns_table_type clox::interpreting::vm::string_object::interns_{};

clox::interpreting::vm::object_type clox::interpreting::vm::string_object::type() const noexcept
{
	return object_type::STRING;
}

std::string clox::interpreting::vm::string_object::string() const
{
	return data_;
}

clox::interpreting::vm::string_object::string_object(std::string value)
		: data_(std::move(value))
{
}

std::string clox::interpreting::vm::string_object::printable_string()
{
	return data_;
}

void string_object::blacken(clox::interpreting::vm::garbage_collector* gc_inst)
{
	// do nothing
}

string_object_raw_pointer
string_object::create_on_heap(const std::shared_ptr<object_heap>& heap, const std::string& value)
{
	if (string_object query_obj{ value };interns_.contains(&query_obj))
	{
		return *interns_.find(&query_obj);
	}
	else
	{
		auto ret = heap->allocate<string_object>(value);
		interns_.insert(ret);
		return ret;
	}
}
