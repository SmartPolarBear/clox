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
// Created by cleve on 10/18/2021.
//

#include "object/class_object.h"
#include "interpreter/vm/garbage_collector.h"

#include "gsl/gsl"

using namespace std;
using namespace gsl;


clox::interpreting::vm::class_object::class_object(std::string name, size_t fields_size)
		: name_(std::move(name)), field_size_(fields_size)
{
}


std::string clox::interpreting::vm::class_object::printable_string()
{
	return name_;
}

clox::interpreting::vm::object_type clox::interpreting::vm::class_object::type() const noexcept
{
	return object_type::OBJECT;
}

void clox::interpreting::vm::class_object::blacken(clox::interpreting::vm::garbage_collector* gc_inst)
{
	for (auto& method: methods_)
	{
		gc_inst->mark_object(method.second);
	}

	for (auto& super: supers_)
	{
		gc_inst->mark_object(super);
	}
}

void clox::interpreting::vm::class_object::put_method(clox::resolving::function_id_type id,
		clox::interpreting::vm::closure_object_raw_pointer closure)
{
	methods_.insert_or_assign(id, closure);
}

bool clox::interpreting::vm::class_object::contains_method(clox::resolving::function_id_type id)
{
	return methods_.contains(id);
}

clox::interpreting::vm::closure_object_raw_pointer
clox::interpreting::vm::class_object::method_at(clox::resolving::function_id_type id)
{
	return methods_.at(id);
}

void clox::interpreting::vm::class_object::inherit(clox::interpreting::vm::class_object* cls)
{
	supers_.push_back(cls);
	for (const auto& m: cls->methods_)
	{
		methods_.insert(m);
	}
}

clox::interpreting::vm::class_object* clox::interpreting::vm::class_object::super(int32_t index)
{
	return supers_.at(index);
}
