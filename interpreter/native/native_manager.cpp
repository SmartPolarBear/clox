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

#include <interpreter/native/native_manager.h>
#include <interpreter/native/native_function_defs.h>

#include <utility>

using namespace clox::interpreter::native;

using namespace std;

id_type native_manager::register_function(const std::string& name, const function_type& function)
{
	auto id = next_id();
	auto func = make_shared<native_function>(name, id, function);
	functions_.insert_or_assign(name, func);
	all_[id] = func;
	return id;
}

id_type native_manager::register_method(const std::string& object_name, const std::string& name, function_type func)
{
	auto id = next_id();
	auto method = make_shared<native_method>(name, id, func);
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
	register_function("clock", nf_clock);
}
