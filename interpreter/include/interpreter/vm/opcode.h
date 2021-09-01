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

#include <cstdint>

namespace clox::interpreting::vm
{
enum class op_code : uint16_t
{
	CONSTANT,
	NIL,
	TRUE,
	FALSE,
	POP,
	GET_LOCAL,
	SET_LOCAL,
	GET_GLOBAL,
	DEFINE_GLOBAL,
	SET_GLOBAL,
	GET_UPVALUE,
	SET_UPVALUE,
	GET_PROPERTY,
	SET_PROPERTY,
	GET_SUPER,
	EQUAL,
	GREATER,
	LESS,
	ADD,
	SUBTRACT,
	MULTIPLY,
	DIVIDE,
	POW,
	MOD,
	NOT,
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
};
}