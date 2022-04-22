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
// Created by cleve on 3/23/2022.
//

#include "../../native/include/native/native_manager.h"
#include "../../native/include/native/native_function_defs.h"

#include <utility>

using namespace clox::interpreting::native;
using namespace clox::resolving;

using namespace std;

id_type native_manager::register_function(const std::string& name,
	const native_function_handle_type& function,
	const std::shared_ptr<clox::resolving::lox_type>& return_type,
	const clox::resolving::lox_callable_type::param_list_type& param_types)
{
	auto id = next_id();
	auto func = make_shared<native_function>(name, id, function, return_type, param_types);
	functions_.insert_or_assign(name, func);
	all_[id] = func;
	return id;
}

id_type native_manager::register_method(const std::string& object_name,
	const std::string& name,
	const native_function_handle_type& func,
	const std::shared_ptr<clox::resolving::lox_type>& return_type,
	const clox::resolving::lox_callable_type::param_list_type& param_types)
{
	auto id = next_id();
	auto method = make_shared<native_method>(name, id, func, return_type, param_types);
	methods_[object_name].insert_or_assign(name, method);
	all_[id] = method;
	return 0;
}

id_type native_manager::next_id()
{
	return ++id_counter_;
}

id_type native_manager::lookup(const std::string& name)
{
	return functions_.at(name)->id();
}

id_type native_manager::lookup(const std::string& object_name, const std::string& name)
{
	return methods_.at(object_name).at(name)->id();
}

std::shared_ptr<native_function> native_manager::get(const std::string& name)
{
	return functions_.at(name);
}

std::shared_ptr<native_method> native_manager::get(const std::string& object_name, const std::string& name)
{
	return methods_.at(object_name).at(name);
}

native_manager::native_manager()
{
	register_global_functions();
}

void native_manager::register_global_functions()
{
	register_function("clock", nf_clock, make_shared<lox_floating_type>(), lox_callable_type::empty_parameter_list());
	register_function("len",
		nf_len,
		make_shared<lox_integer_type>(),
		lox_callable_type::parameter_list_of(make_shared<lox_any_type>()));
}
