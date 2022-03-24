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
// Created by cleve on 8/9/2021.
//

#include <resolver/class_type.h>

#include <utility>

using namespace clox::resolving;

type_id lox_class_type::id_counter_{ PRESET_TYPE_ID_MAX };

clox::resolving::lox_class_type::lox_class_type(std::string name, const std::shared_ptr<lox_object_type>& parent,
		type_map_type fields,
		callable_type_map_type methods,
		type_id id)
		: fields_(std::move(fields)),
		  methods_(std::move(methods)),
		  lox_object_type(std::move(name), ++id_counter_, id, parent)
{
	supers_.push_back(parent);
}

void lox_class_type::put_method(const std::string& name, const std::shared_ptr<parsing::statement>& stmt,
		const std::shared_ptr<lox_callable_type>& func)
{
	if (!methods_.contains(name))
	{
		methods_.insert(std::make_pair(name, std::make_shared<lox_overloaded_metatype>(name)));
	}

	methods_[name]->put(stmt, func);
}
