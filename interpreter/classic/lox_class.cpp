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

#include <interpreter/classic/interpreter.h>

#include <interpreter/classic/lox_function.h>
#include <interpreter/classic/lox_class.h>

#include <resolver/ast_annotation.h>


using namespace std;

using namespace clox::interpreting;
using namespace clox::interpreting::classic;


using namespace clox::resolving;

evaluating_result
lox_class::call(interpreter* the_interpreter,
		const std::shared_ptr<parsing::expression>& caller, const std::vector<evaluating_result>& args)
{
	auto inst = make_shared<lox_instance>(shared_from_this());

	if (auto ctor_ret = lookup_method(scanning::scanner::keyword_from_type(scanning::token_type::CONSTRUCTOR));ctor_ret)
	{
		auto ctor = ctor_ret.value();
		if (auto annotation =caller->get_annotation<call_annotation>();annotation)
		{
			static_pointer_cast<lox_function>(
					ctor.at(annotation->statement()))->bind(
					inst)->call(
					the_interpreter, caller, args);

		}
		else
		{
			throw runtime_error{
					dynamic_pointer_cast<parsing::call_expression>(caller)->get_paren(),
					"Calling non-exist object" };
		}
	}

	return inst;
}

std::string lox_class::printable_string()
{
	return std::format("class {}", name_);
}

std::optional<overloaded_functions>
lox_class::lookup_method(const std::string& name)
{
	if (methods_.contains(name))
	{
		return methods_.at(name);
	}

	if (auto base = base_.lock();base)
	{
		return base->lookup_method(name);
	}

	return nullopt;
}

void
lox_class::put_method(const string& name, const shared_ptr<parsing::statement>& stmt,
		const shared_ptr<struct callable>& func)
{
	methods_[name][stmt] = func;
}
