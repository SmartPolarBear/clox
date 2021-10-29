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
// Created by cleve on 10/18/2021.
//

#pragma once

#include <scanner/scanner.h>

#include <interpreter/vm/object.h>
#include <interpreter/vm/function_object.h>
#include <interpreter/vm/upvalue_object.h>
#include <interpreter/vm/closure_object.h>

#include <resolver/function.h>

#include <variant>
#include <string>

#include <memory>
#include <map>

namespace clox::interpreting::vm
{
class class_object
		: public object
{
public:
	friend class instance_object;

public:
	explicit class_object(std::string name, size_t field_size);

	std::string printable_string() override;

	object_type type() const noexcept override;

	void put_method(resolving::function_id_type id,closure_object_raw_pointer closure);

protected:
	void blacken(struct garbage_collector* gc_inst) override;

private:

	std::string name_{};

	size_t field_size_{};

	std::unordered_map<resolving::function_id_type, closure_object_raw_pointer> methods_{};
};

using class_object_raw_pointer = class_object*;
}