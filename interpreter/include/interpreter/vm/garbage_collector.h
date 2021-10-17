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
// Created by cleve on 10/9/2021.
//

#pragma once

#include <base/iterable_stack.h>

#include <helper/console.h>

#include <interpreter/vm/value.h>
#include <interpreter/vm/heap.h>

namespace clox::interpreting::vm
{
class garbage_collector
{
public:
	friend class gc_recyclable;

	explicit garbage_collector(helper::console& cons, std::shared_ptr<object_heap> heap, class virtual_machine& vm,
			class compiling::codegen& gen);

	void collect();

	void mark_object(object_raw_pointer obj);

	void mark_value(value& val);

private:
	void mark_roots();

	void mark_globals();

	void mark_functions();

	void trace_references();

	void sweep();

	void blacken_object(object_raw_pointer obj);

	std::shared_ptr<object_heap> heap_{ nullptr };

	base::iterable_stack<object_raw_pointer> gray_stack_{};

	mutable class virtual_machine* vm_{ nullptr };

	mutable class compiling::codegen* gen_{ nullptr };

	mutable helper::console* cons_{ nullptr };
};


}