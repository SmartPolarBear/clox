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

#include <scanner/nil_value.h>

#include <interpreter/lox_function.h>
#include <interpreter/interpreter.h>
#include <interpreter/return.h>

#include <memory>

using namespace std;

using namespace clox::interpreting;

size_t clox::interpreting::lox_function::arity()
{
	return decl_->get_params().size();
}

clox::interpreting::evaluating_result
clox::interpreting::lox_function::call(interpreter* the_interpreter, const std::vector<evaluating_result>& args)
{
	auto env = make_shared<environment>(closure_);

	auto params = decl_->get_params();
	for (size_t i = 0; i < params.size(); i++)
	{
		env->put(params[i].lexeme(), args[i]);
	}

	try
	{
		the_interpreter->execute_block(decl_->get_body(), env);
	}
	catch (const return_value& rv)
	{
		if (is_init_)
		{
			auto this_val = closure_->get_at("this", 0);
			if (this_val)
			{
				return this_val.value();
			}
		}
		
		return rv.value();
	}
	catch (...)
	{
		throw;
	}

	if (is_init_)
	{
		auto this_val = closure_->get_at("this", 0);
		if (this_val)
		{
			return this_val.value();
		}
	}

	return scanning::nil_value_tag;
}

std::shared_ptr<lox_function> lox_function::bind(const shared_ptr<lox_instance>& inst)
{
	auto env = make_shared<environment>(closure_);
	env->put("this", inst);
	return make_shared<lox_function>(decl_, env, is_init_);
}
