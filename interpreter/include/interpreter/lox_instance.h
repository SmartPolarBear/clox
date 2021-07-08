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

#include <interpreter/lox_class.h>

#include <memory>
#include <utility>
#include <unordered_map>

namespace clox::interpreting
{
class lox_instance final
		: public helper::printable,
		  public std::enable_shared_from_this<lox_instance>
{
public:
	explicit lox_instance(std::shared_ptr<class lox_class> cls) : class_(std::move(cls))
	{
	}

	std::string printable_string() override;

	evaluating_result get(const scanning::token& tk) const;

	void set(const scanning::token& tk, evaluating_result val);

private:
	std::shared_ptr<class lox_class> class_{ nullptr };
	std::unordered_map<std::string, evaluating_result> fields_{};
};
}
