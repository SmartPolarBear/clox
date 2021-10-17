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
// Created by cleve on 9/7/2021.
//


#include <interpreter/vm/heap.h>
#include <interpreter/vm/exceptions.h>
#include <interpreter/vm/garbage_collector.h>

using namespace clox;
using namespace clox::interpreting;
using namespace clox::interpreting::vm;

object_heap::~object_heap()
{
	while (!objects_.empty())
	{
		auto back = objects_.back();
		objects_.pop_back();
		deallocate(back);
	}
}

clox::interpreting::vm::object_heap::raw_pointer clox::interpreting::vm::object_heap::allocate_raw(size_t size)
{
	if constexpr (base::runtime_predefined_configuration::ENABLE_DEBUG_STRESS_GC)
	{
		if(gc_)
		{
			gc_->collect();
		}
	}

	auto ret = reinterpret_cast<void*>(malloc(size));

	if (!ret)
	{
		throw insufficient_heap_memory{};
	}

	return ret;
}

void clox::interpreting::vm::object_heap::deallocate_raw(object_heap::raw_pointer p)
{
	free(p);
}

void object_heap::use_gc(clox::interpreting::vm::garbage_collector& gc)
{
	gc_ = &gc;
}


