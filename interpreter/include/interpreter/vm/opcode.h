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

#include <helper/enum.h>

#include <cstdint>

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

namespace clox::interpreting::vm
{


enum class op_code : uint16_t
{
	OPCODE_ENUM_MIN,

	CONSTANT,

	// for speed’s sake,
	CONSTANT_NIL,
	CONSTANT_TRUE,
	CONSTANT_FALSE,
	//

	POP,
	POP_N,

	// when codegen for locals and globals,
	// we cannot make sure whether it's assignment or get.
	// so we emit GET_LOCAL/GLOBAL first, and change it to SET_LOCAL/GLOBAL
	// the instance we can make it sure.
	GET_LOCAL,
	SET_LOCAL,

	DEFINE_GLOBAL,
	GET_GLOBAL,
	SET_GLOBAL,

	GET_UPVALUE,
	SET_UPVALUE,
	GET_PROPERTY,
	SET_PROPERTY,
	GET_SUPER,
	EQUAL,
	GREATER,
	LESS,
	GREATER_EQUAL,
	LESS_EQUAL,
	ADD,
	INC, // Add 1
	SUBTRACT,
	DEC, // Subtract 1
	MULTIPLY,
	DIVIDE,
	POW,
	MOD,
	NOT,
	COMMA,
	NEGATE,
	PRINT,
	JUMP,
	JUMP_IF_FALSE,
	LOOP,
	CALL,
	INVOKE,
	SUPER_INVOKE,
	CLOSURE,
	CLOSE_UPVALUE,
	RETURN,
	CLASS,
	INHERIT,
	METHOD,
	LIST_INIT,
	LIST_INIT_RANGE,
	MAP_INIT,
	CONTAINER_GET,
	CONTAINER_SET,
	CONTAINER_GET_RANGE,
	CONTAINER_ITERATE,

	OPCODE_ENUM_MAX,
};

// secondary opcode for inc/dec
enum class inc_dec_secondary_op_code : uint16_t
{
	INC_DEC_OPCODE_ENUM_MIN,

	PREFIX, POSTFIX,

	INC_DEC_OPCODE_ENUM_MAX,
};

static inline constexpr auto op_code_value(op_code code)
{
	return helper::enum_cast(code);
}


static inline constexpr auto op_code_value(inc_dec_secondary_op_code code)
{
	return helper::enum_cast(code);
}


static inline constexpr bool is_patchable(uint16_t code)
{
	return code == op_code_value(op_code::GET_LOCAL) ||
		   code == op_code_value(op_code::GET_GLOBAL);
}

}

#include <magic_enum.hpp>

namespace magic_enum
{
template<>
struct customize::enum_range<clox::interpreting::vm::op_code>
{
	static constexpr int min = (int)clox::interpreting::vm::op_code::OPCODE_ENUM_MIN;
	static constexpr int max = (int)clox::interpreting::vm::op_code::OPCODE_ENUM_MAX;
};
}

namespace magic_enum
{
template<>
struct customize::enum_range<clox::interpreting::vm::inc_dec_secondary_op_code>
{
	static constexpr int min = (int)clox::interpreting::vm::inc_dec_secondary_op_code::INC_DEC_OPCODE_ENUM_MIN;
	static constexpr int max = (int)clox::interpreting::vm::inc_dec_secondary_op_code::INC_DEC_OPCODE_ENUM_MAX;
};
}

#include <interpreter/vm/exceptions.h>

#include <string>
#include <format>

namespace std
{
template<>
struct std::formatter<clox::interpreting::vm::op_code> : std::formatter<std::string>
{
	auto format(clox::interpreting::vm::op_code op, format_context& ctx)
	{
		std::string op_str{ magic_enum::enum_name(op) };

//		switch (op)
//		{
//
//		case clox::interpreting::vm::op_code::CONSTANT:
//			op_str = "CONSTANT";
//			break;
//		case clox::interpreting::vm::op_code::CONSTANT_NIL:
//			op_str = "CONSTANT_NIL";
//			break;
//		case clox::interpreting::vm::op_code::CONSTANT_TRUE:
//			op_str = "TRUE";
//			break;
//		case clox::interpreting::vm::op_code::CONSTANT_FALSE:
//			op_str = "FALSE";
//			break;
//		case clox::interpreting::vm::op_code::POP:
//			op_str = "POP";
//			break;
//		case clox::interpreting::vm::op_code::GET_LOCAL:
//			op_str = "OP_GET_LOCAL";
//			break;
//		case clox::interpreting::vm::op_code::SET_LOCAL:
//			op_str = "SET_LOCAL";
//			break;
//		case clox::interpreting::vm::op_code::GET_GLOBAL:
//			op_str = "GET_GLOBAL";
//			break;
//		case clox::interpreting::vm::op_code::DEFINE_GLOBAL:
//			op_str = "DEFINE_GLOBAL";
//			break;
//		case clox::interpreting::vm::op_code::SET_GLOBAL:
//			op_str = "SET_GLOBAL";
//			break;
//		case clox::interpreting::vm::op_code::GET_UPVALUE:
//			op_str = "GET_UPVALUE";
//			break;
//		case clox::interpreting::vm::op_code::SET_UPVALUE:
//			op_str = "SET_UPVALUE";
//			break;
//		case clox::interpreting::vm::op_code::GET_PROPERTY:
//			op_str = "GET_PROPERTY";
//			break;
//		case clox::interpreting::vm::op_code::SET_PROPERTY:
//			op_str = "SET_PROPERTY";
//			break;
//		case clox::interpreting::vm::op_code::GET_SUPER:
//			op_str = "GET_SUPER";
//			break;
//		case clox::interpreting::vm::op_code::EQUAL:
//			op_str = "EQUAL";
//			break;
//		case clox::interpreting::vm::op_code::GREATER:
//			op_str = "GREATER";
//			break;
//		case clox::interpreting::vm::op_code::LESS:
//			op_str = "LESS";
//			break;
//		case clox::interpreting::vm::op_code::ADD:
//			op_str = "ADD";
//			break;
//		case clox::interpreting::vm::op_code::SUBTRACT:
//			op_str = "SUBTRACT";
//			break;
//		case clox::interpreting::vm::op_code::MULTIPLY:
//			op_str = "MULTIPLY";
//			break;
//		case clox::interpreting::vm::op_code::DIVIDE:
//			op_str = "DIVIDE";
//			break;
//		case clox::interpreting::vm::op_code::POW:
//			op_str = "POW";
//			break;
//		case clox::interpreting::vm::op_code::MOD:
//			op_str = "MOD";
//			break;
//		case clox::interpreting::vm::op_code::NOT:
//			op_str = "NOT";
//			break;
//		case clox::interpreting::vm::op_code::NEGATE:
//			op_str = "NEGATE";
//			break;
//		case clox::interpreting::vm::op_code::PRINT:
//			op_str = "PRINT";
//			break;
//		case clox::interpreting::vm::op_code::JUMP:
//			op_str = "JUMP";
//			break;
//		case clox::interpreting::vm::op_code::JUMP_IF_FALSE:
//			op_str = "JUMP_IF_FALSE";
//			break;
//		case clox::interpreting::vm::op_code::LOOP:
//			op_str = "LOOP";
//			break;
//		case clox::interpreting::vm::op_code::CALL:
//			op_str = "CALL";
//			break;
//		case clox::interpreting::vm::op_code::INVOKE:
//			op_str = "INVOKE";
//			break;
//		case clox::interpreting::vm::op_code::SUPER_INVOKE:
//			op_str = "SUPER_INVOKE";
//			break;
//		case clox::interpreting::vm::op_code::CLOSURE:
//			op_str = "CLOSURE";
//			break;
//		case clox::interpreting::vm::op_code::CLOSE_UPVALUE:
//			op_str = "CLOSE_UPVALUE";
//			break;
//		case clox::interpreting::vm::op_code::RETURN:
//			op_str = "RETURN";
//			break;
//		case clox::interpreting::vm::op_code::CLASS:
//			op_str = "CLASS";
//			break;
//		case clox::interpreting::vm::op_code::INHERIT:
//			op_str = "INHERIT";
//			break;
//		case clox::interpreting::vm::op_code::METHOD:
//			op_str = "METHOD";
//			break;
//		case clox::interpreting::vm::op_code::LIST_INIT:
//			op_str = "LIST_INIT";
//			break;
//		case clox::interpreting::vm::op_code::LIST_INIT_RANGE:
//			op_str = "LIST_INIT_RANGE";
//			break;
//		case clox::interpreting::vm::op_code::MAP_INIT:
//			op_str = "MAP_INIT";
//			break;
//		case clox::interpreting::vm::op_code::CONTAINER_GET:
//			op_str = "CONTAINER_GET";
//			break;
//		case clox::interpreting::vm::op_code::CONTAINER_SET:
//			op_str = "CONTAINER_SET";
//			break;
//		case clox::interpreting::vm::op_code::CONTAINER_GET_RANGE:
//			op_str = "CONTAINER_GET_RANGE";
//			break;
//		case clox::interpreting::vm::op_code::CONTAINER_ITERATE:
//			op_str = "CONTAINER_ITERATE";
//			break;
//		default:
//			throw clox::interpreting::vm::invalid_opcode{ static_cast<uint16_t>(op) };
//		}

		return formatter<string>::format(
				op_str, ctx);
	}
};


template<>
struct std::formatter<clox::interpreting::vm::inc_dec_secondary_op_code> : std::formatter<std::string>
{
	auto format(clox::interpreting::vm::inc_dec_secondary_op_code sop, format_context& ctx)
	{
		std::string op_str{ magic_enum::enum_name(sop) };

		return formatter<string>::format(
				op_str, ctx);
	}
};

}