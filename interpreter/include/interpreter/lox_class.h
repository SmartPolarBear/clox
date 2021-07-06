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
// Created by cleve on 7/6/2021.
//

#pragma once

#include <helper/printable.h>

#include <interpreter/evaluating_result.h>
#include <interpreter/environment.h>

#include <string>

namespace clox::interpreting
{
class lox_class final
		: public callable,
		  public helper::printable
{
public:
	explicit lox_class(const std::string& name) : name_(name)
	{
	}

	size_t arity() override;

	evaluating_result call(struct interpreter* the_interpreter, const std::vector<evaluating_result>& args) override;

	std::string printable_string() override;

	[[nodiscard]] std::string name()const
	{
		return name_;
	}

private:
	std::string name_{};
};
}