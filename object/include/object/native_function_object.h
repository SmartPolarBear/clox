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
// Created by cleve on 3/29/2022.
//

#pragma once



#include "object.h"
#include "../../../native/include/native/native_function.h"

#include <variant>
#include <string>

#include <memory>
#include <map>


namespace clox::interpreting::vm
{
class native_function_object final
		: public object
{
public:

	friend class compiling::codegen;

	explicit native_function_object(std::shared_ptr<native::native_function> func);

	[[nodiscard]] object_type type() const noexcept override;

	[[nodiscard]] std::shared_ptr<native::native_function> function() const
	{
		return func_;
	}

	std::string printable_string() override;

protected:
	void blacken(struct garbage_collector* gc_inst) override;

private:
	std::shared_ptr<native::native_function> func_{};
};

using native_function_object_raw_pointer = native_function_object*;


}
