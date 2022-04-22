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
// Created by cleve on 3/29/2022.
//

#include "object/native_function_object.h"

using namespace clox::interpreting;
using namespace clox::interpreting::native;

vm::native_function_object::native_function_object(std::shared_ptr<native::native_function> func)
		: func_(std::move(func))
{
}

vm::object_type vm::native_function_object::type() const noexcept
{
	return vm::object_type::NATIVE_FUNC;
}

void vm::native_function_object::blacken(clox::interpreting::vm::garbage_collector* gc_inst)
{
}

std::string vm::native_function_object::printable_string()
{
	return std::format("<native function {} at {:x}>", func_->name(), (uintptr_t)func_.get());
}
