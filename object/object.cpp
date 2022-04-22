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
// Created by cleve on 9/6/2021.
//

#include "object/object.h"
#include "object/string_object.h"

using namespace clox::interpreting::vm;

bool clox::interpreting::vm::object::pointer_equal(const clox::interpreting::vm::object* lhs,
		const clox::interpreting::vm::object* rhs)
{
	if (is_string(*lhs) && is_string(*rhs))
	{
		return dynamic_cast<const string_object*>(lhs)->string() == dynamic_cast<const string_object*>(rhs)->string();
	}
	else if (is_string(*lhs) || is_string(*rhs))
	{
		return false;
	}

	return lhs == rhs;
}

