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

#include <helper/console.h>

#include <interpreter/vm/opcode.h>
#include <interpreter/vm/value.h>

#include <scanner/scanner.h>

#include <vector>
#include <optional>

namespace clox::interpreting::vm
{
class chunk final
{
public:
	static inline constexpr int64_t INVALID_LINE = -1;

	using code_type = uint16_t;

	using iterator_type = std::vector<uint16_t>::iterator;
public:
	chunk() = default;

	explicit chunk(std::string name);

	~chunk() = default;

	chunk(const chunk&) = default;

	chunk(chunk&&) = default;

	chunk& operator=(const chunk&) = default;

	auto begin()
	{
		return codes_.begin();
	}

	auto end()
	{
		return codes_.end();
	}

	void disassemble(helper::console& out);

	void write(code_type op, std::optional<scanning::token> t = std::nullopt);

	void write(code_type op, int64_t line);

	code_type add_constant(const value& val);

private:

	uint64_t disassemble_instruction(helper::console& out, uint64_t offset);


	std::string name_{};

	std::vector<value> constants_{};

	std::vector<code_type> codes_{};
	std::vector<int64_t> lines_{};
};
}