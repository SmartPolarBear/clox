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
// Created by cleve on 3/23/2022.
//

#include "object/map_object.h"

#include "interpreter/vm/garbage_collector.h"

#include <utility>

clox::interpreting::vm::map_object::map_object(std::vector<std::pair<value, value>> vals)
	: values_(std::move(vals))
{
}

std::string clox::interpreting::vm::map_object::printable_string()
{
	return std::format("map object at {}, containing {} pairs.", reinterpret_cast<uintptr_t>(this), values_.size());
}

clox::interpreting::vm::object_type clox::interpreting::vm::map_object::type() const noexcept
{
	return clox::interpreting::vm::object_type::MAP;
}

void clox::interpreting::vm::map_object::blacken(clox::interpreting::vm::garbage_collector* gc_inst)
{
	for (auto& val : values_)
	{
		gc_inst->mark_value(val.first);
		gc_inst->mark_value(val.second);
	}
}

size_t clox::interpreting::vm::map_object::size() const
{
	return values_.size();
}
