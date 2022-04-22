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

#include "interpreter/vm/value.h"
#include "native.h"

#include "type/lox_type.h"
#include "type/callable_type.h"

#include <string_view>
#include <functional>

#include "gsl/gsl"

namespace clox::interpreting::native
{
class native_function
{
 public:
	friend class native_manager;

	[[nodiscard]] explicit native_function(std::string name,
		id_type id,
		native_function_handle_type func,
		std::shared_ptr<clox::resolving::lox_type> return_type,
		clox::resolving::lox_callable_type::param_list_type param_types);

	native_function(const native_function&) = default;

	native_function(native_function&&) = default;

	native_function& operator=(const native_function&) = default;

	[[nodiscard]] std::string name() const
	{
		return name_;
	}

	[[nodiscard]] id_type id() const
	{
		return id_;
	}

	virtual value_type call(std::vector<value_type> args);

	[[nodiscard]] std::shared_ptr<clox::resolving::lox_type> return_type() const
	{
		return return_type_;
	}

	[[nodiscard]] clox::resolving::lox_callable_type::param_list_type parameter_types() const
	{
		return param_types_;
	}

 protected:

	native_function_handle_type function_;

 private:
	id_type id_{};
	std::string name_{};

	std::shared_ptr<clox::resolving::lox_type> return_type_{};
	clox::resolving::lox_callable_type::param_list_type param_types_{};
};
}