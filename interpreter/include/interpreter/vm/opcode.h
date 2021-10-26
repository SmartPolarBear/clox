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

using full_opcode_type = uint32_t;
using main_opcode_base_type [[maybe_unused]] = uint16_t;
using secondary_opcode_base_type = uint16_t;

static inline constexpr size_t SECONDARY_LSHIFT = 16;
static inline constexpr full_opcode_type SECONDARY_MASK = 0xFFFF0000;
static inline constexpr full_opcode_type MAIN_MASK = 0xFFFF;

enum class op_code : main_opcode_base_type
{
	OPCODE_ENUM_MIN,

	CONSTANT,

	// for speed’s sake,
	CONSTANT_NIL,
	CONSTANT_TRUE,
	CONSTANT_FALSE,
	//

	PUSH,
	POP,
	POP_N,

	// when codegen for locals and globals,
	// we cannot make sure whether it's assignment or get.
	// so we emit GET_LOCAL/GLOBAL first, and change it to SET_LOCAL/GLOBAL
	// the instance we can make it sure.
	GET,
	SET,
	DEFINE,

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
enum secondary_op_code : secondary_opcode_base_type
{
	SEC_OPCODE_ENUM_MIN,

	SEC_OP_PREFIX = 1 << 0,
	SEC_OP_POSTFIX = 1 << 1,
	SEC_OP_GLOBAL = 1 << 2,
	SEC_OP_LOCAL = 1 << 3,
	SEC_OP_UPVALUE = 1 << 4,
	SEC_OP_FUNC = 1 << 5,
	SEC_OP_CLASS = 1 << 5,
	SEC_OP_CAPTURE = 1 << 6,

	SEC_OPCODE_ENUM_MAX,
};

static inline constexpr full_opcode_type compose_opcode(secondary_opcode_base_type sec, op_code main)
{
	auto ret = (static_cast<full_opcode_type>(sec) << SECONDARY_LSHIFT) |
			   static_cast<full_opcode_type>(helper::enum_cast(main));

	return ret;
}

static inline constexpr full_opcode_type patch_main(full_opcode_type op, op_code main)
{
	op &= SECONDARY_MASK;
	op |= static_cast<full_opcode_type>(helper::enum_cast(main));
	return op;
}

static inline constexpr full_opcode_type patch_secondary(full_opcode_type op, secondary_opcode_base_type sec)
{
	op &= MAIN_MASK;
	op |= static_cast<full_opcode_type>(sec << SECONDARY_LSHIFT);
	return op;
}

static inline constexpr op_code main_op_code_of(full_opcode_type code)
{
	return static_cast<op_code>(code & MAIN_MASK);
}

static inline constexpr secondary_opcode_base_type secondary_op_code_of(full_opcode_type code)
{
	return static_cast<secondary_opcode_base_type>(code >> SECONDARY_LSHIFT);
}


static inline constexpr auto op_code_value(op_code code)
{
	return helper::enum_cast(code);
}


static inline constexpr auto op_code_value(secondary_op_code code)
{
	return helper::enum_cast(code);
}


static inline constexpr bool is_patchable(full_opcode_type code)
{
	return main_op_code_of(code) == op_code::GET;
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
struct std::formatter<clox::interpreting::vm::secondary_op_code> : std::formatter<std::string>
{
	auto format(clox::interpreting::vm::secondary_op_code sop, format_context& ctx)
	{
		return formatter<string>::format(
				to_string(clox::helper::enum_cast(sop)), ctx);
	}
};

}