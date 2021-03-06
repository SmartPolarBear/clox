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
// Created by cleve on 10/3/2021.
//

#pragma once

#include "object.h"
#include "interpreter/vm/value.h"
#include <optional>


namespace clox::interpreting::vm
{
class upvalue_object final
		: public object
{
public:
	explicit upvalue_object(value* val)
			: value_(val)
	{
	}

	std::string printable_string() override;

	[[nodiscard]] object_type type() const noexcept override;

	void close();

	value* get_value() const;

private:
	value* value_{ nullptr };
protected:
	void blacken(struct garbage_collector* gc_inst) override;

private:

	std::optional<value> closed_{ std::nullopt };
};

using upvalue_object_raw_pointer = upvalue_object*;

}