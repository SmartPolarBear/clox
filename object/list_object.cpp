// Copyright (c) 2022 SmartPolarBear
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
// Created by cleve on 3/19/2022.
//

#include "interpreter/vm/garbage_collector.h"
#include "object/list_object.h"

#include <utility>

clox::interpreting::vm::list_object::list_object(std::vector<value> values)
		: values_(std::move(values))
{

}

std::string clox::interpreting::vm::list_object::printable_string()
{
	return std::format("list object at {}, containing {} objects.", reinterpret_cast<uintptr_t>(this), values_.size());
}

clox::interpreting::vm::object_type clox::interpreting::vm::list_object::type() const noexcept
{
	return object_type::LIST;
}

void clox::interpreting::vm::list_object::blacken(clox::interpreting::vm::garbage_collector* gc_inst)
{
	for (auto& val: values_)
	{
		gc_inst->mark_value(val);
	}
}

clox::interpreting::vm::value
clox::interpreting::vm::list_object::get(clox::interpreting::vm::list_object::index_type idx) const
{
	return values_.at(idx);
}

void clox::interpreting::vm::list_object::set(clox::interpreting::vm::list_object::index_type idx,
		clox::interpreting::vm::value val)
{
	values_.at(idx) = std::move(val);
}


size_t clox::interpreting::vm::list_object::size() const
{
	return values_.size();
}
