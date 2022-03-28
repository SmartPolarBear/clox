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

#pragma once

#include <resolver/lox_type.h>
#include <resolver/object_type.h>
#include <resolver/callable_type.h>

#include <unordered_map>

namespace clox::resolving
{
class lox_class_type
		: public lox_object_type
{
public:
	using type_map_type = std::unordered_map<std::string, std::shared_ptr<lox_type>>;
	using callable_type_map_type = std::unordered_map<std::string, std::shared_ptr<lox_overloaded_metatype>>;

	using super_list_type = std::vector<std::shared_ptr<lox_object_type>>;
public:
	[[nodiscard]] explicit lox_class_type(std::string name, const std::shared_ptr<lox_object_type>& parent,
			type_map_type fields = type_map_type{},
			callable_type_map_type methods = callable_type_map_type{});

	[[nodiscard]] explicit lox_class_type(std::string name, const std::shared_ptr<lox_object_type>& parent,
			type_id id,
			type_map_type fields = type_map_type{},
			callable_type_map_type methods = callable_type_map_type{});

	[[nodiscard]] type_map_type& fields()
	{
		return fields_;
	}

	[[nodiscard]] callable_type_map_type& methods()
	{
		return methods_;
	}

	void put_method(const std::string& name, const std::shared_ptr<parsing::statement>& stmt,
			const std::shared_ptr<lox_callable_type>& func);


private:
	type_map_type fields_{};
	callable_type_map_type methods_{};

	// To support multiple super (one base, multiple interfaces) in the future
	super_list_type supers_{};

	static type_id id_counter_;
};
}
