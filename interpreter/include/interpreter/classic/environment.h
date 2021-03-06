// Copyright (c) 2021 SmartPolarBear
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
// Created by cleve on 6/19/2021.
//

#pragma once

#include <interpreter/classic/evaluating_result.h>
#include <interpreter/classic/runtime_error.h>

#include <unordered_map>
#include <string>
#include <optional>
#include <memory>

namespace clox::interpreting::classic
{
class environment final
		: public std::enable_shared_from_this<environment>
{
public:
	using value_map_type = std::unordered_map<std::string, evaluating_result>;
	using func_map_type = std::unordered_map<std::string, overloaded_functions>;

	environment() : parent_(),
					values_(std::make_shared<value_map_type>()),
					funcs_(std::make_shared<func_map_type>())
	{

	}

	explicit environment(const std::shared_ptr<environment>& parent)
			: parent_(parent),
			  values_(std::make_shared<value_map_type>()),
			  funcs_(std::make_shared<func_map_type>())
	{

	}

	std::shared_ptr<environment> parent()
	{
		return parent_.lock();
	}

	std::optional<evaluating_result> get(const scanning::token& name);

	std::optional<evaluating_result> get_at(const std::string& name, int64_t depth);

	void put(const std::string& name, evaluating_result value);

	void
	put(const std::string& name, const std::shared_ptr<parsing::statement>&, const std::shared_ptr<class callable>&);

	void assign(const scanning::token& name, evaluating_result val);

	void assign_at(const scanning::token& name, evaluating_result val, int64_t depth);


private:
	std::weak_ptr<environment> ancestor(int64_t dist);

	std::shared_ptr<value_map_type> values_{};

	// if we directly use values_, MSVC will report strange error. Work it around
	std::shared_ptr<func_map_type> funcs_{};

	std::weak_ptr<environment> parent_{};
};
}