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

#include <interpreter/native/native_function.h>

#include <utility>

using namespace std;

clox::interpreting::native::native_function::native_function(std::string name,
	clox::interpreting::native::id_type id,
	clox::interpreting::native::native_function_handle_type func,
	std::shared_ptr<clox::resolving::lox_type> return_type,
	clox::resolving::lox_callable_type::param_list_type param_types)
	: name_(std::move(name)), id_(id), function_(std::move(func)),
	  return_type_(std::move(return_type)), param_types_(std::move(param_types))
{
}

clox::interpreting::native::value_type
clox::interpreting::native::native_function::call(std::vector<value_type> args)
{
	return function_(std::nullopt, std::move(args));
}
