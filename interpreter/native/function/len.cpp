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
// Created by cleve on 4/20/2022.
//

#include "interpreter/native/native_function_defs.h"

#include <interpreter/vm/object.h>
#include <interpreter/vm/list_object.h>
#include <interpreter/vm/map_object.h>

#include <variant>

using namespace std;

using namespace clox::interpreting::native;
using namespace clox::interpreting::vm;

value_type clox::interpreting::native::nf_len([[maybe_unused]]std::optional<value_type> self,
	[[maybe_unused]] std::vector<value_type> args)
{
	auto arg = args.front();

	if (holds_alternative<vm::object_value_type>(arg))
	{
		auto obj = get<vm::object_value_type>(arg);

		switch (obj->type())
		{
		case object_type::LIST:
		{
			auto list = reinterpret_cast<list_object_raw_pointer>(obj);
			return list->size();
		}
		case object_type::MAP:
		{
			auto map = reinterpret_cast<map_object_raw_pointer>(obj);
			return map->size();
		}
		default:
			break;
		}
	}

	return 0; // TODO: throw an exception
};