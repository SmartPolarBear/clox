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
// Created by cleve on 9/24/2021.
//

#pragma once

#include "scanner/scanner.h"

#include "object.h"
#include "function_object.h"
#include "upvalue_object.h"

#include <variant>
#include <string>

#include <memory>
#include <map>

namespace clox::interpreting::vm
{
class closure_object final
		: public object
{
protected:
	void blacken(struct garbage_collector* gc_inst) override;

public:
	[[nodiscard]] explicit closure_object(function_object_raw_pointer func);

	std::string printable_string() override;

	[[nodiscard]] object_type type() const noexcept override;

	[[nodiscard]] function_object_raw_pointer function() const
	{
		return function_;
	}

	[[nodiscard]] std::vector<upvalue_object_raw_pointer>& upvalues() const
	{
		return upvalues_;
	}

private:
	function_object_raw_pointer function_{};

	mutable std::vector<upvalue_object_raw_pointer> upvalues_{};
};

using closure_object_raw_pointer = closure_object*;
}