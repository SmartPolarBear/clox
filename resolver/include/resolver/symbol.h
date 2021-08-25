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

#pragma once

#include <scanner/scanner.h>

#include <parser/parser.h>

#include <resolver/lox_type.h>

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <unordered_map>
#include <compare>
#include <stdexcept>

namespace clox::resolving
{

enum class symbol_type
{
	ST_NAMED = 1,
	ST_FUNCTION
};

class symbol
{
public:
	[[nodiscard]] virtual symbol_type symbol_type() const = 0;

	[[nodiscard]] virtual std::shared_ptr<lox_type> type() const = 0;
};

// TODO: add function_symbol or something likely

class function_multi_symbol
		: public symbol
{
public:
	[[nodiscard]] resolving::symbol_type symbol_type() const override;

	[[nodiscard]] std::shared_ptr<lox_type> type() const override;
};

class named_symbol
		: public symbol
{
public:
	named_symbol() = default;

	~named_symbol() = default;

	explicit named_symbol(std::string name, std::shared_ptr<lox_type> type);

	[[nodiscard]] resolving::symbol_type symbol_type() const override;

	[[nodiscard]] std::shared_ptr<lox_type> type() const override;

private:
	std::string name_{};
	std::shared_ptr<lox_type> type_{};
};


}