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
// Created by cleve on 10/30/2021.
//

#pragma once

#include <scanner/scanner.h>

#include <interpreter/vm/object.h>
#include <interpreter/vm/function_object.h>
#include <interpreter/vm/upvalue_object.h>
#include <interpreter/vm/class_object.h>

#include <variant>
#include <string>

#include <memory>
#include <map>

#include <gsl/gsl>

namespace clox::interpreting::vm
{
class bounded_method_object
		: public object
{
public:

	explicit bounded_method_object(value receiver, closure_object_raw_pointer method);

	std::string printable_string() override;

	[[nodiscard]] object_type type() const noexcept override;

	[[nodiscard]] value receiver() const
	{
		return receiver_;
	}

	[[nodiscard]] closure_object_raw_pointer method() const
	{
		return method_;
	}

protected:
	void blacken(struct garbage_collector* gc_inst) override;

private:
	value receiver_{};
	closure_object_raw_pointer method_{};
};

using bounded_method_object_pointer = bounded_method_object*;
}
