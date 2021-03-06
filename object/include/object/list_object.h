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
// Created by cleve on 3/19/2022.
//

#pragma once

#include "scanner/scanner.h"

#include "object.h"
#include "interpreter/vm/value.h"

#include <variant>
#include <string>

#include <memory>
#include <map>
#include "gsl/gsl"

namespace clox::interpreting::vm
{
class list_object
		: public  object
{
public:
	explicit list_object(std::vector<value> values);

	using index_type = gsl::index;

	std::string printable_string() override;

	[[nodiscard]] object_type type() const noexcept override;

	value get(index_type idx) const;

	void set(index_type idx, value val);

	size_t size()const;

protected:
	void blacken(struct garbage_collector* gc_inst) override;

private:
	std::vector<value> values_{};
};

using list_object_raw_pointer = class list_object*;

}

