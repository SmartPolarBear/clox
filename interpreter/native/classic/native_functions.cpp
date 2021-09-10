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

#include <cstddef>
#include <chrono>

#include <interpreter/classic/interpreter.h>
#include <interpreter/classic/evaluating_result.h>
#include <interpreter/classic/native_functions.h>

using namespace clox::interpreting;
using namespace clox::interpreting::classic;

using namespace std::chrono;


evaluating_result
clox::interpreting::classic::clock_func::call(struct interpreter* the_interpreter,
		const std::shared_ptr<parsing::expression>& caller,
		const std::vector<evaluating_result>& args)
{
	return (long double)duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}
