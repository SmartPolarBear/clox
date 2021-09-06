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
#include <interpreter/vm/chunk.h>
#include <interpreter/vm/value.h>

#include <memory>

namespace clox::interpreting::vm
{

enum class virtual_machine_status
{
	OK,
	RUNTIME_ERROR,
};

class vm_return final
{
public:
	explicit vm_return(virtual_machine_status st)
			: status_(st)
	{
	}

	[[nodiscard]] virtual_machine_status status() const
	{
		return status_;
	}

private:
	virtual_machine_status status_{};
};


class virtual_machine final
{
public:
	static inline constexpr size_t STACK_RESERVED_SIZE = 64;
public:
	virtual_machine() = delete;

	explicit virtual_machine(helper::console& cons);

private:
	virtual_machine_status run();

	bool run_code(chunk::code_type instruction);

	template<class ...TArgs>
	void runtime_error(std::string_view fmt, const TArgs& ...args)
	{
		cons_->error() << std::format("[Line {}] in file {}:",
				chunk_->line_of(ip_),
				chunk_->filename()) << std::endl;

		cons_->error() << std::format(fmt, args...);

		reset_stack();
	}

	template<typename TOp>
	requires std::invocable<TOp, scanning::floating_literal_type, scanning::floating_literal_type>
	inline void binary_op(TOp op)
	{
		try
		{
			auto right = get_number_promoted(peek(0));
			auto left = get_number_promoted(peek(1));

			pop_two_and_push(op(left, right));
		}
		catch (const std::exception& e)
		{
			this->runtime_error("Invalid operands for binary operator: {}", e.what());
		}
	}

	void reset_stack();

	value peek(size_t offset = 0);

	value pop();

	void push(const value& val);

	inline void pop_two_and_push(const value& val);

	value next_constant();

	chunk::code_type next_byte();

	std::shared_ptr<chunk> chunk_{};	chunk::iterator_type ip_{};

	std::vector<value> stack_{};

	mutable helper::console* cons_{ nullptr };
};
}
