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
// Created by cleve on 10/6/2021.
//

#include <interpreter/vm/upvalue_object.h>
#include "interpreter/vm/garbage_collector.h"

std::string clox::interpreting::vm::upvalue_object::printable_string()
{
	return std::format("Upvalue to {:x}", (uintptr_t)value_);
}

clox::interpreting::vm::object_type clox::interpreting::vm::upvalue_object::type() const noexcept
{
	return object_type::UPVALUE;
}

clox::interpreting::vm::value* clox::interpreting::vm::upvalue_object::get_value() const
{
	return value_;
}

void clox::interpreting::vm::upvalue_object::close()
{
	closed_ = *value_;
	value_ = &closed_.value();
}

void clox::interpreting::vm::upvalue_object::blacken(clox::interpreting::vm::garbage_collector* gc_inst)
{
	if(closed_.has_value())
	{
		gc_inst->mark_value(this->closed_.value());
	}
}
