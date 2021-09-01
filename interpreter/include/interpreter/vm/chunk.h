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
// Created by cleve on 9/1/2021.
//

#pragma once

#include <interpreter/vm/opcode.h>

#include <scanner/scanner.h>

#include <vector>
#include <optional>

namespace clox::interpreting::vm
{
class chunk final
{
public:
	static inline constexpr int64_t INVALID_LINE = -1;
public:
	chunk() = default;

	~chunk() = default;

	chunk(const chunk&) = default;

	chunk(chunk&&) = default;

	chunk& operator=(const chunk&) = default;

	void add_op(uint16_t op, std::optional<scanning::token> t = std::nullopt);

private:
	std::vector<uint16_t> codes_{};
	std::vector<int64_t> lines_{};
};
}