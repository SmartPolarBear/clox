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
#include <helper/enum.h>

#include "type/object_type.h"
#include "type/instance_type.h"
#include "type/initializer_list_type.h"

#include <tuple>
#include <utility>
#include <format>

using namespace clox;

using namespace clox::helper;

using namespace clox::resolving;


std::shared_ptr<lox_object_type> resolving::lox_object_type::super() const
{
	return super_.lock();
}

uint64_t lox_object_type::depth() const
{
	return depth_;
}

bool resolving::lox_object_type::operator<(const resolving::lox_type& another) const
{
	if (!dynamic_cast<const lox_object_type*>(&another))
		return false;

	const auto& obj = dynamic_cast<const lox_object_type&>(another);

	return *this < obj;
}

bool lox_object_type::operator<(const lox_object_type& obj) const
{
	if (is_primitive(*this) && is_primitive(obj))
	{
		return this->id() <= obj.id(); // this is a hack
	}

	if (this->depth() < obj.depth())
	{
		return false;
	}
	else if (this->depth() == obj.depth())
	{
		return this->id() == obj.id();
	}

	auto pa = this->super();
	for (; pa && pa->depth() <= obj.depth(); pa = pa->super())
	{
		if (pa->id() == obj.id())
		{
			return true;
		}
	}

	return false;
}

lox_object_type::lox_object_type(std::string name, type_id id, uint64_t flags,
		const std::shared_ptr<lox_object_type>& parent)
		: name_(std::move(name)),
		  id_(id),
		  flags_(TYPE_CLASS | flags),
		  super_(parent),
		  depth_(parent ? parent->depth() + 1 : 0)
{
}

std::string lox_object_type::printable_string()
{
	return std::format("<class {}>", name_);
}

uint64_t lox_object_type::flags() const
{
	return flags_;
}

type_id lox_object_type::id() const
{
	return id_;
}


std::vector<std::shared_ptr<lox_object_type>>& lox_object_type::derived()
{
	return derived_;
}

std::shared_ptr<lox_object_type> lox_object_type::object()
{
	static std::shared_ptr<lox_object_type> inst{ nullptr };
	if (!inst)
	{
		inst = std::make_shared<lox_object_type>("object", PRIMITIVE_TYPE_ID_OBJECT, TYPE_PRIMITIVE, nullptr);
	}
	return inst;
}

std::shared_ptr<lox_object_type> lox_object_type::integer()
{
	static std::shared_ptr<lox_object_type> inst{ nullptr };
	if (!inst)
	{
		inst = std::make_shared<lox_integer_type>();
		object()->derived_.push_back(inst);
	}

	return inst;
}

std::shared_ptr<lox_object_type> lox_object_type::floating()
{
	static std::shared_ptr<lox_object_type> inst{ nullptr };
	if (!inst)
	{
		inst = std::make_shared<lox_floating_type>();
		object()->derived_.push_back(inst);
	}
	return inst;
}

std::shared_ptr<lox_object_type> lox_object_type::boolean()
{
	static std::shared_ptr<lox_object_type> inst{ nullptr };
	if (!inst)
	{
		inst = std::make_shared<lox_boolean_type>();
		object()->derived_.push_back(inst);
	}
	return inst;
}

std::shared_ptr<lox_object_type> lox_object_type::nil()
{
	static std::shared_ptr<lox_object_type> inst{ nullptr };
	if (!inst)
	{
		inst = std::make_shared<lox_nil_type>();
		object()->derived_.push_back(inst);
	}
	return inst;
}

std::shared_ptr<lox_object_type> lox_object_type::string()
{
	static std::shared_ptr<lox_object_type> inst{ nullptr };
	if (!inst)
	{
		inst = std::make_shared<lox_string_type>();
		object()->derived_.push_back(inst);
	}
	return inst;
}

bool lox_object_type::operator==(const lox_type& another) const
{
	if (lox_type::is_instance(another))
	{
		return operator==(*dynamic_cast<const lox_instance_type&>(another).underlying_type());
	}

	return id() == another.id();
}

bool lox_object_type::operator!=(const lox_type& another) const
{
	return !(*this == another);
}

std::string lox_object_type::name() const
{
	return this->name_;
}


lox_integer_type::lox_integer_type()
		: lox_object_type("integer", PRIMITIVE_TYPE_ID_INTEGER, TYPE_PRIMITIVE, lox_object_type::object())
{
}


lox_floating_type::lox_floating_type()
		: lox_object_type("floating", PRIMITIVE_TYPE_ID_FLOATING, TYPE_PRIMITIVE, lox_object_type::object())
{
}


lox_boolean_type::lox_boolean_type()
		: lox_object_type("boolean", PRIMITIVE_TYPE_ID_BOOLEAN, TYPE_PRIMITIVE, lox_object_type::object())
{
}


lox_nil_type::lox_nil_type()
		: lox_object_type("nil", PRIMITIVE_TYPE_ID_NIL, TYPE_PRIMITIVE, lox_object_type::object())
{
}


lox_string_type::lox_string_type()
		: lox_object_type("string", TYPE_ID_STRING_CLASS, TYPE_CLASS, lox_object_type::object())
{
}
