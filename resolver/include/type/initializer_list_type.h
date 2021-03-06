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
// Created by cleve on 8/29/2021.
//

#pragma once

#include "lox_type.h"

#include "object_type.h"
#include "callable_type.h"
#include "class_type.h"

#include <unordered_map>

namespace clox::resolving
{

class [[deprecated]] initializer_list_type
		: public lox_type
{
public:
	explicit initializer_list_type(std::vector<std::shared_ptr<lox_type>> items);

	std::string printable_string() override;

	uint64_t flags() const override;

	type_id id() const override;

	bool operator<(const lox_type& target) const override;

	bool operator==(const lox_type& lox_type) const override;

	bool operator!=(const lox_type& lox_type) const override;

private:
	std::vector<std::shared_ptr<lox_type>> items_{};
};

}
