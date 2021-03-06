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
#include <interpreter/vm/exceptions.h>

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

	if (line == INVALID_LINE)
	{
		lines_.push_back(last_line_);
	}
	else
	{
		lines_.push_back(line);
		last_line_ = line;
	}

}


uint64_t clox::interpreting::vm::chunk::disassemble_instruction(helper::console& out, uint64_t offset)
{
	auto op = main_op_code_of(codes_[offset]);
	auto secondary = secondary_op_code_of(codes_[offset]);

	out.log() << std::format("{0:0>8}", offset); // example: 00000001:	CONSTANT

	constexpr size_t LINE_NUMBER_WIDTH = 10;
	if (offset > 0 && lines_[offset] == lines_[offset - 1])
	{
		out.log() << std::format("{0:>{1}}  ", "|", LINE_NUMBER_WIDTH);
	}
	else
	{
		if (lines_[offset] == INVALID_LINE)
		{
			out.log() << std::format("{0:>{1}}  ", "<invalid>", LINE_NUMBER_WIDTH);
		}
		else
		{
			out.log() << std::format("{0:>{1}}  ", lines_[offset], LINE_NUMBER_WIDTH);
		}
	}

	try
	{
		out.log() << std::format("[{0:0>8b}, <{1:>8}>{2:0>8}]", // len: 1+8+8+1+8+1
				secondary,
				op,
				helper::enum_cast(op));
	}
	catch (const invalid_opcode&)
	{
		out.log() << std::format("{0:>27}", "<INVALID>");
	}


	switch (op)
	{
	case op_code::CONSTANT:
		out.log() << std::format(" {} '{}'", codes_[offset + 1], constants_[codes_[offset + 1]]) << endl;
		return offset + 2;

	case op_code::POP_N:
		out.log() << std::format(" N={}", codes_[offset + 1]) << endl;
		return offset + 2;

	case op_code::JUMP:
	case op_code::JUMP_IF_FALSE:
		out.log() << std::format(" {} -> {}", offset, offset + 2 + codes_[offset + 1]) << endl;
		return offset + 2;

	case op_code::INC:
	case op_code::DEC:
	case op_code::SET:
	case op_code::GET:
	case op_code::DEFINE:
		if (secondary & SEC_OP_GLOBAL)
		{
			out.log() << std::format(" {} '{}'", codes_[offset + 1], constants_[codes_[offset + 1]]) << endl;
		}
		else if (secondary & SEC_OP_LOCAL)
		{
			out.log() << std::format(" (stack slot) '{}'", codes_[offset + 1]) << endl;
		}
		else if (secondary & SEC_OP_UPVALUE)
		{
			out.log() << std::format(" Upvalue {}", codes_[offset + 1]) << endl;
		}
		else if (secondary & SEC_OP_FUNC)
		{
			out.log() << std::format(" ID={}, constant {} '{}'", codes_[offset + 1], codes_[offset + 2],
					constants_[codes_[offset + 2]]) << endl;
			return offset + 3;
		}

		return offset + 2;

	case op_code::PUSH:
		if (secondary & SEC_OP_FUNC)
		{
			out.log() << std::format(" ID={}", codes_[offset + 1]) << endl;
			return offset + 2;
		}
		else if (secondary & SEC_OP_CLASS)
		{
			out.log() << std::format(" Name={}", constants_[codes_[offset + 1]]) << endl;
			return offset + 2;
		}

	case op_code::LOOP:
		out.log() << std::format(" (offset) '{}'", codes_[offset + 1]) << endl;
		return offset + 2;


	case op_code::CALL:
		out.log() << std::format(" {} args",
				codes_[offset + 1]) << endl;
		return offset + 2;

	case op_code::CLOSURE:
	{
		if (secondary & SEC_OP_CAPTURE)
		{
			offset++;
			auto count = codes_[offset++];
			for (int i = 0; i < count; i++)
			{
				auto local = codes_[offset++];
				auto index = codes_[offset++];
				out.log() << std::format(" {} {}{}",
						local ? "Local" : "Upvalue", index, i == count - 1 ? "" : ",");
			}
			out.log() << endl;
			return offset;
		}
		else
		{
			out.log() << " (No captures specified)" << endl;
			return offset + 1;
		}
	}

	case op_code::CLASS:
		out.log() << std::format(" {} '{}', {} fields", codes_[offset + 1], constants_[codes_[offset + 1]],
				codes_[offset + 2]) << endl;
		return offset + 3;

	case op_code::GET_PROPERTY:
		if (secondary & SEC_OP_FUNC)
		{
			out.log() << std::format(" Function ID {}", codes_[offset + 1]) << endl;
		}
		else
		{
			out.log() << std::format(" Member offset {}", codes_[offset + 1]) << endl;
		}
		return offset + 2;

	case op_code::SET_PROPERTY:
		out.log() << std::format(" Member offset {}", codes_[offset + 1]) << endl;
		return offset + 2;

	case op_code::METHOD:
		out.log() << std::format(" ID= {}", codes_[offset + 1]) << endl;
		return offset + 2;

	case op_code::INVOKE:
		out.log() << std::format(" ID= {}, {} args", codes_[offset + 1], codes_[offset + 2]) << endl;
		return offset + 3;

	case op_code::GET_SUPER:
		if (secondary & SEC_OP_LOCAL)
		{
			out.log() << std::format("{}th base, {}th member", codes_[offset + 1], codes_[offset + 2]) << endl;
		}
		else if (secondary & SEC_OP_FUNC)
		{
			out.log() << std::format("{}th base, method ID: {}", codes_[offset + 1], codes_[offset + 2]) << endl;
		}
		return offset + 3;

	case op_code::MAKE_LIST:
		out.log() << std::format(" Size= {} ", codes_[offset + 1]) << endl;
		return offset + 2;

	case op_code::INSTANCE:
		if (secondary & SEC_OP_FUNC)
		{
			out.log() << std::format(" ID= {}, {} args.", codes_[offset + 1], codes_[offset + 2]) << endl;
			return offset + 3;
		}
		[[fallthrough]];
	default:
		out.log() << endl;
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
	if (constants_.size() > numeric_limits<full_opcode_type>::max())
	{
		throw too_many_constants{};
	}

	return constants_.size() - 1;
}

value& chunk::constant_at(code_type pos)
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

void chunk::patch_begin(chunk::code_type new_op, int64_t offset)
{
	*(codes_.begin() + offset) = new_op;
}

void chunk::patch_end(chunk::code_type new_op, int64_t offset)
{
	*(codes_.rbegin() + offset) = new_op;
}


chunk::code_type chunk::peek(int64_t offset)
{
	return *(codes_.rbegin() + offset);
}
