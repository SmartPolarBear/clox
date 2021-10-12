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

#include <interpreter/vm/object.h>

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

	friend class garbage_collector;

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
		return ret;
	}

	template<std::derived_from<object> T>
	void deallocate(T* val)
	{
		delete val;
	}


private:
	raw_pointer allocate_raw(size_t size);

	void deallocate_raw(raw_pointer raw);

	object_list_type objects_{};

	mutable helper::console* cons_{};

};


}
