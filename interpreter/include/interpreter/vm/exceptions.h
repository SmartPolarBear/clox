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
// Created by cleve on 9/2/2021.
//

#pragma once

#include <interpreter/vm/opcode.h>

#include <interpreter/vm/value.h>


#include <stdexcept>
#include <format>

namespace clox::interpreting::vm
{

class insufficient_heap_memory final
		: public std::runtime_error
{
public:
	insufficient_heap_memory()
			: std::runtime_error("Insufficient heap memory.")
	{
	}
};

class invalid_opcode final
		: public std::invalid_argument
{
public:
	explicit invalid_opcode(full_opcode_type opcode)
			: opcode_(opcode),
			  std::invalid_argument(std::format("Invalid opcode {}", opcode))
	{
	}

private:
	full_opcode_type opcode_{};
};

class invalid_value final
		: public std::invalid_argument
{
public:
	explicit invalid_value(const value& val)
			: value_(val),
			  std::invalid_argument(std::format("invalid value {}", val))
	{
	}

private:
	value value_;
};

class too_many_constants final
		: public std::runtime_error
{
public:
	too_many_constants() : std::runtime_error("Too many constants")
	{
	}
};

class runtime_error
		: public std::runtime_error
{
public:

	runtime_error(int64_t line, const std::string& msg)
			: line_(line), std::runtime_error(msg)
	{
	}

	~runtime_error() override = default;

private:
	int64_t line_;
};


}