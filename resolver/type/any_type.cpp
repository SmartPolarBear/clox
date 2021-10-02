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
// Created by cleve on 8/30/2021.
//

#include "helper/enum.h"

#include "resolver/lox_type.h"
#include "resolver/instance_type.h"

#include <tuple>
#include <utility>
#include <format>

using namespace clox;

using namespace clox::helper;

using namespace clox::resolving;

uint64_t lox_any_type::flags() const
{
	return TYPE_PRIMITIVE;
}

type_id lox_any_type::id() const
{
	return PRIMITIVE_TYPE_ID_ANY;
}

bool lox_any_type::operator<(const lox_type& target) const
{
	return true;
}

std::string lox_any_type::printable_string()
{
	return "<any type>";
}

bool lox_any_type::operator==(const lox_type& t) const
{
	return true;
}

bool lox_any_type::operator!=(const lox_type& lox_type) const
{
	return false;
}

