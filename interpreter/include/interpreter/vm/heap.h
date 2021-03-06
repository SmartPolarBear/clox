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

#pragma once

#include <helper/console.h>
#include <helper/std_console.h>

#include <base/predefined.h>

#include <scanner/scanner.h>

#include "object/object.h"

#include <variant>
#include <string>

#include <memory>
#include <map>


namespace clox::interpreting::vm
{

class object_heap
{
public:
	using object_list_type = std::list<object_raw_pointer>;

	using raw_pointer = void*;

	using size_type = size_t;

	friend class garbage_collector;

	// TODO: use runtime configuration
	static inline constexpr size_type NEXT_GC_INITIAL = 1024;

public:
	object_heap() = delete;

	explicit object_heap(helper::console& cons)
			: cons_(&cons)
	{
	}

	~object_heap();

	template<std::derived_from<object> T, class ...Args>
	T* allocate(Args&& ...args)
	{
		using TRaw = std::decay_t<T>;
		raw_pointer mem = allocate_raw(sizeof(TRaw));

		auto ret = new(mem) T(std::forward<Args>(args)...); // placement new
		objects_.push_back(ret);

		if constexpr (base::runtime_predefined_configuration::ENABLE_DEBUG_LOGGING_GC)
		{
			cons_->log()
					<< std::format("At {:x} allocate {} bytes for type {}", (uintptr_t)mem, sizeof(TRaw), ret->type())
					<< std::endl;
		}

		return ret;
	}

	template<class T>
	requires std::derived_from<T, object> || std::same_as<T, object_raw_pointer>
	void deallocate(T* val)
	{
		using TRaw = std::decay_t<T>;

		if constexpr (base::runtime_predefined_configuration::ENABLE_DEBUG_LOGGING_GC)
		{
			cons_->log()
					<< std::format("At {:x} deallocate {} bytes of type {}", (uintptr_t)val, sizeof(*val), val->type())
					<< std::endl;
		}
		deallocate_raw(val, sizeof(TRaw));
	}

	object_heap& enable_gc(class garbage_collector& gc);

	object_heap& remove_gc();

private:
	raw_pointer allocate_raw(size_t size);

	void deallocate_raw(raw_pointer raw, size_t size);

	object_list_type objects_{};

	size_type size_{ 0 };

	size_type next_gc_{ NEXT_GC_INITIAL };

	mutable class garbage_collector* gc_{};

	mutable helper::console* cons_{};

};


}
