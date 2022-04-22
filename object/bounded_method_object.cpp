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
// Created by cleve on 10/30/2021.
//

#include "object/class_object.h"
#include "interpreter/vm/garbage_collector.h"
#include "object/bounded_method_object.h"

#include "gsl/gsl"
#include <utility>

using namespace std;
using namespace gsl;

using namespace clox::interpreting::vm;

clox::interpreting::vm::bounded_method_object::bounded_method_object(
		value receiver,
		clox::interpreting::vm::closure_object_raw_pointer method)
		: receiver_(std::move(receiver)), method_(method)
{
}

std::string clox::interpreting::vm::bounded_method_object::printable_string()
{
	return method_->printable_string();
}

clox::interpreting::vm::object_type clox::interpreting::vm::bounded_method_object::type() const noexcept
{
	return clox::interpreting::vm::object_type::BOUNDED_METHOD;
}

void clox::interpreting::vm::bounded_method_object::blacken(clox::interpreting::vm::garbage_collector* gc_inst)
{
	gc_inst->mark_value(receiver_);
	gc_inst->mark_object(method_);
}
