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

#include "base/base.h"

#include "interpreter/native/native.h"
#include "interpreter/native/native_function.h"
#include "interpreter/native/native_method.h"

#include "resolver/callable_type.h"

#include <memory>

namespace clox::interpreting::native
{
class native_manager
		: public base::singleton<native_manager>
{
public:
	native_manager();

	id_type register_function(const std::string& name, const function_type& function);

	id_type register_method(const std::string& object_name, const std::string& name, function_type method);

	id_type lookup(const std::string& name);

	id_type lookup(const std::string& object_name, const std::string& name);

	std::shared_ptr<native_function> get(const std::string& name);

	std::shared_ptr<native_method> get(const std::string& object_name, const std::string& name);

	template<typename T>
	requires std::is_same_v<T, native_function> || std::is_same_v<T, native_method>
	inline std::shared_ptr<T> get(id_type id)
	{
		return std::static_pointer_cast<T>(all_.at(id));
	}

	std::unordered_map<std::string, std::shared_ptr<native_function>>& functions()
	{
		return functions_;
	}

private:
	void register_global_functions();

	id_type next_id();

	std::unordered_map<std::string, std::shared_ptr<native_function> > functions_{};
	std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<native_method> >> methods_{};

	std::unordered_map<id_type, std::shared_ptr<native_function>> all_{};

	id_type id_counter_{ 1 };
};


}
