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
// Created by cleve on 8/14/2021.
//

#pragma once

#include <scanner/scanner.h>

namespace clox::resolving
{
static inline constexpr auto OVERRIDABLE_OPS = {
		clox::scanning::token_type::BANG, clox::scanning::token_type::BANG_EQUAL,
		clox::scanning::token_type::EQUAL, clox::scanning::token_type::EQUAL_EQUAL,
		clox::scanning::token_type::GREATER, clox::scanning::token_type::GREATER_EQUAL,
		clox::scanning::token_type::LESS, clox::scanning::token_type::LESS_EQUAL,
		clox::scanning::token_type::MINUS, clox::scanning::token_type::MINUS_MINUS,
		clox::scanning::token_type::PLUS, clox::scanning::token_type::PLUS_PLUS,
		clox::scanning::token_type::STAR, clox::scanning::token_type::STAR_STAR,
		clox::scanning::token_type::ARROW,
		clox::scanning::token_type::COMMA, clox::scanning::token_type::DOT,
		clox::scanning::token_type::AND, clox::scanning::token_type::OR
};
}
