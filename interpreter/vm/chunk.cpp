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

void clox::interpreting::vm::chunk::add_op(uint16_t op, std::optional<scanning::token> t)
{
	codes_.push_back(op);

	if (t.has_value())
	{
		lines_.push_back(t->line());
	}
	else
	{
		lines_.push_back(INVALID_LINE);
	}
}

uint64_t clox::interpreting::vm::chunk::disassemble_instruction(helper::console& out, uint64_t offset)
{
	auto op = static_cast<op_code>(codes_[offset]);
	out.out() << std::format("{0:8}:	{1}", offset, op); // example: 00000001:	CONSTANT

	if (offset > 0 && lines_[offset] == lines_[offset - 1])
	{
		out.out() << "    | ";
	}
	else
	{
		out.out() << std::format("{0:4} ", lines_[offset]);
	}

	switch (op)
	{
	case op_code::CONSTANT:
		out.out() << std::format(" {} '{}'", codes_[offset + 1],lines_[codes_[offset + 1]]);
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

uint16_t chunk::add_constant(const value& val)
{
	constants_.push_back(val);
	return constants_.size() - 1;
}
