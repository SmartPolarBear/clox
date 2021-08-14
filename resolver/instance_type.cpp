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
// Created by cleve on 8/12/2021.
//

#include <resolver/instance_type.h>

#include <utility>
#include <format>

std::string clox::resolving::lox_instance_type::printable_string()
{
	return std::format("instance of type {}", type_->printable_string());
}

uint64_t clox::resolving::lox_instance_type::flags() const
{
	return FLAG_INSTANCE;//| TYPE_CLASS;
}

clox::resolving::type_id clox::resolving::lox_instance_type::id() const
{
	return TYPE_ID_INSTANCE;
}

bool clox::resolving::lox_instance_type::operator<(const clox::resolving::lox_type& target) const
{
	return type_ && type_->operator<(target);
}

bool clox::resolving::lox_instance_type::operator==(const clox::resolving::lox_type& target) const
{
	return type_ && type_->operator==(target);

}

bool clox::resolving::lox_instance_type::operator!=(const clox::resolving::lox_type& target) const
{
	return type_ && type_->operator!=(target);
}

clox::resolving::lox_instance_type::lox_instance_type(std::shared_ptr<lox_object_type> obj)
		: type_(std::move(obj))
{
}
