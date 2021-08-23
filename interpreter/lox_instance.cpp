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
#include <utility>

#include <interpreter/lox_instance.h>
#include <interpreter/runtime_error.h>


using namespace std;

using namespace clox::interpreting;

std::string clox::interpreting::lox_instance::printable_string()
{
	return std::format("Instance of class {} at {}", class_->name(), (uintptr_t)this);
}

clox::interpreting::evaluating_result clox::interpreting::lox_instance::get(const clox::scanning::token& tk) const
{
	if (fields_.contains(tk.lexeme()))
	{
		return fields_.at(tk.lexeme());
	}

	if (auto m = class_->lookup_method(tk.lexeme());m)
	{
		auto method = m.value();

		for (auto& p:method)
		{
			p.second = dynamic_pointer_cast<lox_function>(p.second)->bind(
					const_pointer_cast<lox_instance>(shared_from_this()));
		}

		return method;
	}

	throw runtime_error{ tk, std::format("{} property is undefined.", tk.lexeme()) };
}

void lox_instance::set(const clox::scanning::token& tk, evaluating_result val)
{
	fields_[tk.lexeme()] = std::move(val);
}

