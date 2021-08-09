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
// Created by cleve on 8/8/2021.
//
#pragma once

#include <resolver/lox_type.h>

namespace clox::resolving
{

class lox_object_type
		: public lox_type,
		  public std::enable_shared_from_this<lox_object_type>
{
public:
	static std::shared_ptr<lox_object_type> object();

	bool operator!=(const lox_type& lox_type) const override;

	bool operator==(const lox_type&) const override;

	static std::shared_ptr<lox_object_type> integer();

	static std::shared_ptr<lox_object_type> floating();

	static std::shared_ptr<lox_object_type> boolean();

	static std::shared_ptr<lox_object_type> nil();

	static std::shared_ptr<lox_object_type> string();

public:
	std::string printable_string() override;

	uint64_t flags() const override;

	type_id id() const override;

	explicit lox_object_type(std::string name, type_id id, uint64_t flags,
			const std::shared_ptr<lox_object_type>& parent);

	std::shared_ptr<lox_object_type> super() const;

	std::vector<std::shared_ptr<lox_object_type>>& derived();

	uint64_t depth() const;

	bool operator<(const lox_type&) const override;

	bool operator<(const lox_object_type&) const;

private:
	std::string name_;

	type_id id_{ 0 };

	std::weak_ptr<lox_object_type> super_{};
	std::vector<std::shared_ptr<lox_object_type>> derived_{};

	uint64_t depth_{ 0 };
protected:
	uint64_t flags_{ 0 };
};

class lox_integer_type final
		: public lox_object_type
{
public:
	lox_integer_type();
};

class lox_floating_type final
		: public lox_object_type
{
public:
	lox_floating_type();
};

class lox_boolean_type final
		: public lox_object_type
{
public:
	lox_boolean_type();
};

class lox_nil_type final
		: public lox_object_type
{
public:
	lox_nil_type();
};

class lox_string_type final
		: public lox_object_type
{
public:
	lox_string_type();
};

}

