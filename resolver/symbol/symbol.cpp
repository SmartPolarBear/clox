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
// Created by cleve on 8/4/2021.
//

#include <resolver/symbol.h>


using namespace clox::resolving;


symbol_type named_symbol::symbol_type() const
{
	return resolving::symbol_type::ST_NAMED;
}

std::shared_ptr<lox_type> named_symbol::type() const
{
	return type_;
}

named_symbol::named_symbol(std::string name, std::shared_ptr<lox_type> type)
		: name_(std::move(name)), type_(std::move(type))
{
}

named_symbol::named_symbol(std::string name, std::shared_ptr<lox_type> type,named_symbol::named_symbol_type t, int64_t slot_index)
		: name_(std::move(name)), type_(std::move(type)), symbol_type_(t), slot_index_(slot_index)
{
}

symbol_type function_multi_symbol::symbol_type() const
{
	return resolving::symbol_type::ST_FUNCTION;
}

std::shared_ptr<lox_type> function_multi_symbol::type() const
{
	throw std::logic_error{ "function_multi_symbol should not use type()" };
}
