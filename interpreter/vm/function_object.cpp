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
// Created by cleve on 9/14/2021.
//

#include <interpreter/vm/function_object.h>
#include <interpreter/vm/chunk.h>

#include <utility>

using namespace std;
using namespace clox::interpreting::vm;

clox::interpreting::vm::object_type clox::interpreting::vm::function_object::type() const noexcept
{
	return object_type::FUNCTION;
}

clox::interpreting::vm::function_object::function_object(std::string name, size_t arity)
		: name_(std::move(name)), arity_(arity), body_(make_shared<chunk>())
{
}

string clox::interpreting::vm::function_object::printable_string()
{
	return std::format("<function {} of {} arity>", name_, arity_);
}
