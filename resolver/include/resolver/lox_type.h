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
#include <vector>

namespace clox::resolving
{

enum lox_type_flags : uint64_t
{
	TYPE_PRIMITIVE = 1,
	TYPE_CLASS = 2,
	TYPE_VOID = 4,
	FLAG_CALLABLE = 8,
	FLAG_CTOR = 16,
	FLAG_INSTANCE = 32,
};


using type_id = int64_t;
using type_id_diff = int64_t;

enum primitive_type_id : type_id
{
	PRIMITIVE_TYPE_ID_VOID = -1,

	PRIMITIVE_TYPE_ID_ANY = 0,

	// the order represent if it can be assigned !
	PRIMITIVE_TYPE_ID_NIL,
	PRIMITIVE_TYPE_ID_OBJECT,
	PRIMITIVE_TYPE_ID_BOOLEAN,
	PRIMITIVE_TYPE_ID_INTEGER,
	PRIMITIVE_TYPE_ID_FLOATING,

	TYPE_ID_STRING_CLASS,
	TYPE_ID_INSTANCE,

	TYPE_ID_OVERLOADED_FUNC,

	PRESET_TYPE_ID_MAX,
};


struct type_deduce_defer_tag
{
};

static inline constexpr type_deduce_defer_tag type_deduce_defer{};

class lox_type
		: public helper::printable
{
public:
	static bool is_primitive(const lox_type& t)
	{
		return t.flags() & lox_type_flags::TYPE_PRIMITIVE;
	}

	static bool is_callable(const lox_type& t)
	{
		return t.flags() & lox_type_flags::FLAG_CALLABLE;
	}

	static bool is_class(const lox_type& t)
	{
		return t.flags() & lox_type_flags::TYPE_CLASS;
	}

	static bool is_instance(const lox_type& t)
	{
		return t.flags() & lox_type_flags::FLAG_INSTANCE;
	}

	static bool unify(const lox_type& base, const lox_type& derived);

	static std::shared_ptr<lox_type>
	intersect(const std::shared_ptr<lox_type>& t1, const std::shared_ptr<lox_type>& t2);

public:

	virtual uint64_t flags() const = 0;

	[[nodiscard]] virtual type_id id() const = 0;

	/// if this is a subtype of target.
	/// \param target
	/// \return true if this is a subtype of target.
	virtual bool operator<(const lox_type& target) const = 0;

	virtual bool operator==(const lox_type&) const = 0;

	virtual bool operator!=(const lox_type&) const = 0;
};

class lox_any_type final
		: public lox_type
{
public:
	bool operator!=(const lox_type& lox_type) const override;

public:
	uint64_t flags() const override;

	bool operator==(const lox_type& lox_type) const override;

	std::string printable_string() override;

	[[nodiscard]] type_id id() const override;

	bool operator<(const lox_type& target) const override;
};

class lox_void_type final
		: public lox_type
{
public:
	std::string printable_string() override;

	uint64_t flags() const override;

	type_id id() const override;

	bool operator<(const lox_type& target) const override;

	bool operator==(const lox_type& lox_type) const override;

	bool operator!=(const lox_type& lox_type) const override;
};

}