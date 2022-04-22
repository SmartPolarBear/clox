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
// Created by cleve on 10/18/2021.
//

#pragma once

#include "scanner/scanner.h"

#include "object.h"
#include "function_object.h"
#include "upvalue_object.h"
#include "class_object.h"

#include <variant>
#include <string>

#include <memory>
#include <map>

#include "gsl/gsl"

namespace clox::interpreting::vm
{
class instance_object
		: public object
{
public:
	using index_type = gsl::index;

	explicit instance_object(class_object_raw_pointer class_obj);

	std::string printable_string() override;

	[[nodiscard]] object_type type() const noexcept override;

	void set(index_type idx, const value&);

	[[nodiscard]] value get(index_type idx) const;

	[[nodiscard]] class_object_raw_pointer class_object() const
	{
		return class_;
	}

protected:
	void blacken(struct garbage_collector* gc_inst) override;


private:
	class_object_raw_pointer class_{};
	std::vector<value> fields_{};
};

using instance_object_raw_pointer = instance_object*;
}
