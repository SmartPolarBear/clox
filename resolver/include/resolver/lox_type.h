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

#include <base.h>
#include <helper/printable.h>

#include <tuple>
#include <memory>
#include <compare>

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

using type_id = uint64_t;

enum primitive_type_id : type_id
{
	PRIMITIVE_TYPE_ID_ERROR = 0,

	PRIMITIVE_TYPE_ID_INTEGER,
	PRIMITIVE_TYPE_ID_FLOATING,
	PRIMITIVE_TYPE_ID_BOOLEAN,
	PRIMITIVE_TYPE_ID_NIL,


	PRIMITIVE_TYPE_ID_MAX,
};

class lox_type
		: public helper::printable
{
public:
	static bool is_error(lox_type& t)
	{
		return t.flags() & lox_type_flags::TYPE_ERROR;
	}

	static bool is_primitive(lox_type& t)
	{
		return t.flags() & lox_type_flags::TYPE_PRIMITIVE;
	}

public:

	virtual uint64_t flags() = 0;

	virtual type_id id() = 0;

	virtual size_t size() = 0;

};


class error_type final
		: public lox_type
{
public:
	type_id id() override
	{
		return PRIMITIVE_TYPE_ID_ERROR;
	}

	uint64_t flags() override;

	std::string printable_string() override;

	size_t size() override;
};

class nil_type final
		: public lox_type
{
public:
	type_id id() override
	{
		return PRIMITIVE_TYPE_ID_NIL;
	}

	std::string printable_string() override;

	uint64_t flags() override;

	size_t size() override;
};

class integer_type final
		: public lox_type
{
public:
	type_id id() override
	{
		return PRIMITIVE_TYPE_ID_INTEGER;
	}

	std::string printable_string() override;

	uint64_t flags() override;

	size_t size() override;
};

class floating_type final
		: public lox_type
{
public:
	type_id id() override
	{
		return PRIMITIVE_TYPE_ID_FLOATING;
	}

	std::string printable_string() override;

	uint64_t flags() override;

	size_t size() override;
};


class boolean_type final
		: public lox_type
{
public:
	type_id id() override
	{
		return PRIMITIVE_TYPE_ID_BOOLEAN;
	}

	std::string printable_string() override;

	uint64_t flags() override;

	size_t size() override;
};

}