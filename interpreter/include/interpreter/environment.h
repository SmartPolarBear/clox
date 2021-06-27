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

#include <interpreter/evaluating_result.h>
#include <interpreter/runtime_error.h>

#include <unordered_map>
#include <string>
#include <optional>
#include <memory>

namespace clox::interpreting
{
class environment final
{
public:
	environment() = default;

	explicit environment(const std::shared_ptr<environment>& parent)
			: parent_(parent)
	{

	}

	std::optional<evaluating_result> get(const scanning::token& name);

	void put(const std::string& name, evaluating_result value);

	void assign(const scanning::token& name, evaluating_result val);

private:
	std::unordered_map<std::string, evaluating_result> values_{};

	std::weak_ptr<environment> parent_{};
};
}