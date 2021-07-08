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

#include <format>
#include <memory>

#include <interpreter/lox_function.h>
#include <interpreter/lox_class.h>

using namespace std;

using namespace clox::interpreting;

size_t clox::interpreting::lox_class::arity()
{
	return 0;
}

clox::interpreting::evaluating_result
clox::interpreting::lox_class::call(struct interpreter* the_interpreter, const std::vector<evaluating_result>& args)
{
	return make_shared<lox_instance>(shared_from_this());
}

std::string clox::interpreting::lox_class::printable_string()
{
	return std::format("class {}", name_);
}

std::shared_ptr<lox_function> clox::interpreting::lox_class::lookup_method(const std::string& name)
{
	if (methods_.contains(name))
	{
		return methods_.at(name);
	}

	return nullptr;
}
