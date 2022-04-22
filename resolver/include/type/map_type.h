// Copyright (c) 2022 SmartPolarBear
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
// Created by cleve on 3/17/2022.
//

#pragma once

#include "lox_type.h"

#include "object_type.h"
#include "callable_type.h"
#include "class_type.h"

#include <unordered_map>

namespace clox::resolving
{
class lox_map_type final
		: public lox_object_type
{
public:
	explicit lox_map_type(const std::shared_ptr<lox_type>& key, const std::shared_ptr<lox_type>& val);

	std::shared_ptr<lox_type> key_type() const
	{
		return key_type_;
	}


	std::shared_ptr<lox_type> value_type() const
	{
		return value_type_;
	}

private:
	std::shared_ptr<lox_type> key_type_{ nullptr };
	std::shared_ptr<lox_type> value_type_{ nullptr };
};

}
