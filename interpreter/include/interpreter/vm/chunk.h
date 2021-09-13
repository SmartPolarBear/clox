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

	using code_type = full_opcode_type;
	static_assert(sizeof(code_type) >= sizeof(uint32_t));

	using code_list_type = std::vector<code_type>;

	using iterator_type = code_list_type::iterator;
	using difference_type = int64_t;
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

	auto count()
	{
		return codes_.size();
	}

	void disassemble(helper::console& out);

	void write(code_type op, std::optional<scanning::token> t = std::nullopt);

	void write(code_type op, int64_t line);

	/// Patch the code offset from the first opcode
	/// \param new_op
	/// \param offset
	void patch_begin(code_type new_op, int64_t offset);

	/// Patch the code offset from the last opcode
	/// \param new_op
	/// \param offset
	void patch_end(code_type new_op, int64_t offset);

	code_type peek(int64_t offset);

	code_type add_constant(const value& val);

	value constant_at(code_type pos);

	int64_t line_of(code_list_type::iterator ip);

	std::string filename();

private:

	uint64_t disassemble_instruction(helper::console& out, uint64_t offset);


	std::string name_{};

	std::vector<value> constants_{};

	code_list_type codes_{};
	std::vector<int64_t> lines_{};
};
}