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
// Created by cleve on 7/18/2021.
//

#pragma once

#include <tuple>

namespace clox::resolving
{

enum lox_type_flags : uint64_t
{
	TYPE_PRIMITIVE = 1,
	TYPE_CLASS = 2,
	TYPE_ERROR = 4,
};

enum class lox_primitive_type_size : size_t
{
	INTEGER = 8,
	FLOATING = 8,
	BOOLEAN = INTEGER,
	NIL = INTEGER,
};

class lox_type
{
public:
	static bool is_error(lox_type& t)
	{
		return t.type() & lox_type_flags::TYPE_ERROR;
	}

public:
	virtual lox_type_flags type() = 0;

	virtual size_t size() = 0;
};

class error_type final
		: public lox_type
{
public:
	lox_type_flags type() override;

	size_t size() override;
};

class nil_type final
		: public lox_type
{
public:
	lox_type_flags type() override;

	size_t size() override;
};

class integer_type final
		: public lox_type
{
public:
	lox_type_flags type() override;

	size_t size() override;
};

class floating_type final
		: public lox_type
{
public:
	lox_type_flags type() override;

	size_t size() override;
};


class boolean_type final
		: public lox_type
{
public:
	lox_type_flags type() override;

	size_t size() override;
};

}