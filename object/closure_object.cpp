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
// Created by cleve on 9/24/2021.
//

#include "object/closure_object.h"
#include "interpreter/vm/garbage_collector.h"

clox::interpreting::vm::closure_object::closure_object(function_object_raw_pointer func)
		: function_{ func }
{
	func->wrapper_closure_ = this;
}

std::string clox::interpreting::vm::closure_object::printable_string()
{
	return function_->printable_string(); // Closure is transparent to user, meaning it is identical to its function
}

clox::interpreting::vm::object_type clox::interpreting::vm::closure_object::type() const noexcept
{
	return object_type::CLOSURE;
}

void clox::interpreting::vm::closure_object::blacken(clox::interpreting::vm::garbage_collector* gc_inst)
{
	gc_inst->mark_object(function_);
	for (auto& upvalue: upvalues_)
	{
		gc_inst->mark_object(upvalue);
	}
}
