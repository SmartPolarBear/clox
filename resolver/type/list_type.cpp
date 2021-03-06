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
// Created by cleve on 3/17/2022.
//

#include "type/list_type.h"

using namespace std;

clox::resolving::lox_list_type::lox_list_type(const std::shared_ptr<lox_type>& elem)
		: lox_class_type(std::format("list[{}]", elem->printable_string()), lox_object_type::object(), TYPE_ID_LIST, {},
		{}),
		  element_type_(elem)
{
	auto append_method = make_shared<lox_overloaded_metatype>("append");
	auto append_callable = make_shared<lox_callable_type>("append", make_shared<lox_void_type>(),
			lox_callable_type::param_list_type{ make_pair(nullopt, element_type_) }, false, true);


	append_method->put(nullptr, append_callable);
	methods()["append"] = append_method;
}
