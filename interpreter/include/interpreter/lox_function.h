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
// Created by cleve on 6/30/2021.
//
#pragma once

#pragma once

#include <interpreter/evaluating_result.h>
#include <interpreter/environment.h>

#include <parser/gen/parser_classes.inc>


#include <memory>
#include <utility>

namespace clox::interpreting
{
class lox_function final
		: public callable
{
public:
	explicit lox_function(std::shared_ptr<parsing::function_statement> decl)
			: decl_(std::move(decl))
	{
	}

	lox_function(std::shared_ptr<parsing::function_statement> decl, std::shared_ptr<environment> closure)
			: decl_(std::move(decl)), closure_(std::move(closure))
	{
	}

	~lox_function() = default;

	size_t arity() override;

	evaluating_result call(class interpreter* intp, const std::vector<evaluating_result>& args) override;

private:
	std::shared_ptr<environment> closure_{ nullptr };
	std::shared_ptr<parsing::function_statement> decl_{ nullptr };
};
}