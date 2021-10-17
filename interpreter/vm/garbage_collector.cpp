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
#include <helper/std_console.h>

#include <base/predefined.h>

#include <interpreter/codegen/codegen.h>

#include <interpreter/vm/garbage_collector.h>
#include <interpreter/vm/vm.h>

#include <format>
#include <gsl/gsl>

using namespace std;
using namespace gsl;

using namespace clox;
using namespace clox::base;
using namespace clox::interpreting::vm;

garbage_collector::garbage_collector(helper::console& cons, std::shared_ptr<object_heap> heap,
		virtual_machine& vm, compiling::codegen& cg)
		: cons_(&cons), heap_(std::move(heap)), vm_(&vm), gen_(&cg)
{
}

void clox::interpreting::vm::garbage_collector::collect()
{
	if constexpr(runtime_predefined_configuration::ENABLE_DEBUG_LOGGING_GC)
	{
		cons_->log() << "-- begin gc" << endl;
	}

	mark_roots();

	trace_references();

	sweep();


	if constexpr(runtime_predefined_configuration::ENABLE_DEBUG_LOGGING_GC)
	{
		cons_->log() << "-- end gc" << endl;
	}
}

void garbage_collector::mark_roots()
{
	for (auto& val: vm_->stack_)
	{
		mark_value(val);
	}

	for (auto& call_frame: vm_->call_frames_)
	{
		mark_object(call_frame.closure());
	}

	for (auto& upvalue: vm_->open_upvalues_)
	{
		mark_object(upvalue.second);
	}

	mark_globals();

	mark_functions();
}

void garbage_collector::mark_globals()
{
	for (auto& p: vm_->globals_)
	{
		mark_value(p.second);
	}
}

void garbage_collector::mark_functions()
{
	for (auto& func: vm_->functions_)
	{
		mark_value(func.second);
	}

	for (auto& func: gen_->functions_)
	{
		mark_object(func);
	}
}

void garbage_collector::mark_value(value& val)
{
	std::visit([this](auto&& val)
	{
		using T = std::decay_t<decltype(val)>;
		if constexpr (std::is_same_v<T, object_value_type>)
		{
			mark_object(val);
		}
		else
		{
			return; // do nothing
		}
	}, val);
}


void garbage_collector::mark_object(object_raw_pointer obj)
{
	if (obj == nullptr)return;
	if (obj->marked_)return;

	obj->marked_ = true;

	gray_stack_.push(obj);
}

void garbage_collector::trace_references()
{
	while (!gray_stack_.empty())
	{
		auto top = gray_stack_.top();
		gray_stack_.pop();

		blacken_object(top);
	}
}

void garbage_collector::blacken_object(object_raw_pointer obj)
{
	if constexpr (base::runtime_predefined_configuration::ENABLE_DEBUG_LOGGING_GC)
	{
		cons_->log() << std::format("At {:x} blacken {}", (uintptr_t)obj, obj->printable_string())
					 << std::endl;
	}

	obj->blacken(this);
}

void garbage_collector::sweep()
{
	// remove white things in string table
	for (auto iter = string_object::interns_.begin(); iter != string_object::interns_.end();)
	{
		if ((*iter)->marked_)
		{
			iter++;
		}
		else
		{
			auto* unreachable = *iter;
			iter = string_object::interns_.erase(iter);
			heap_->deallocate(unreachable);
		}
	}

	for (auto iter = heap_->objects_.begin(); iter != heap_->objects_.end();)
	{
		if ((*iter)->marked_)
		{
			(*iter)->marked_ = false;
			iter++;
		}
		else
		{
			auto unreached = *iter;
			iter = heap_->objects_.erase(iter);
			heap_->deallocate(unreached);
		}
	}
}