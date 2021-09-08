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

#include <interpreter/vm/chunk.h>
#include <interpreter/vm/opcode.h>

using namespace std;

using namespace clox::interpreting;
using namespace clox::interpreting::vm;


chunk::chunk(std::string name)
		: name_(std::move(name))
{

}

void clox::interpreting::vm::chunk::write(code_type op, std::optional<scanning::token> t)
{
	if (t.has_value())
	{
		write(op, t->line());
	}
	else
	{
		write(op, INVALID_LINE);
	}
}


void chunk::write(code_type op, int64_t line)
{
	codes_.push_back(op);
	lines_.push_back(line);
}


uint64_t clox::interpreting::vm::chunk::disassemble_instruction(helper::console& out, uint64_t offset)
{
	auto op = static_cast<op_code>(codes_[offset]);
	out.out() << std::format("{0:0>8}", offset); // example: 00000001:	CONSTANT

	if (offset > 0 && lines_[offset] == lines_[offset - 1])
	{
		out.out() << "        | ";
	}
	else
	{
		if (lines_[offset] == INVALID_LINE)
		{
			out.out() << std::format("{0:8}  ", "<invalid>");
		}
		else
		{
			out.out() << std::format("{0:8}  ", lines_[offset]);
		}
	}

	try
	{
		out.out() << std::format("{}", op);
	}
	catch (const invalid_opcode&)
	{
		out.out() << "<INVALID>";
	}

	switch (op)
	{
	case op_code::SET_GLOBAL:
	case op_code::GET_GLOBAL:
	case op_code::DEFINE_GLOBAL:
	case op_code::CONSTANT:
		out.out() << std::format(" {} '{}'", codes_[offset + 1], constants_[codes_[offset + 1]]) << endl;
		return offset + 2;

	case op_code::INC:
	case op_code::DEC:
		out.out() << std::format(" {}", codes_[offset + 1]) << endl;
		return offset + 2;


	case op_code::GET_LOCAL:
	case op_code::SET_LOCAL:
		out.out() << std::format(" (stack slot) '{}'", codes_[offset + 1]) << endl;
		return offset + 2;

	default:
		out.out() << endl;
		return offset + 1;
	}
}

void chunk::disassemble(helper::console& out)
{
	for (uint64_t offset = 0; offset < codes_.size();)
	{
		offset = disassemble_instruction(out, offset);
	}
}

chunk::code_type chunk::add_constant(const value& val)
{
	constants_.push_back(val);
	if (constants_.size() > UINT16_MAX)
	{
		throw too_many_constants{};
	}

	return constants_.size() - 1;
}

value chunk::constant_at(chunk::code_type pos)
{
	return constants_.at(pos);
}

int64_t chunk::line_of(chunk::code_list_type::iterator ip)
{
	return lines_.at(ip - begin() - 1);
}

std::string chunk::filename()
{
	return "<filename placeholder>";
}

void chunk::patch(chunk::code_type new_op, int64_t offset)
{
	*(codes_.rbegin() + offset) = new_op;
}

chunk::code_type chunk::peek(int64_t offset)
{
	return *(codes_.rbegin() + offset);
}
