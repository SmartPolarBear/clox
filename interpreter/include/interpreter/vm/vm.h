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
#include <interpreter/vm/heap.h>

#include <memory>
#include "string_object.h"

namespace clox::interpreting::vm
{

enum class virtual_machine_status
{
	OK,
	RUNTIME_ERROR,
};

class virtual_machine final
{
public:
	static inline constexpr size_t CALL_STACK_RESERVED_SIZE = 64;
	static inline constexpr size_t STACK_RESERVED_SIZE = 16384;

	using value_list_type = std::vector<value>;
	using global_table_type = std::unordered_map<std::string, value>;
	using function_table_type = std::unordered_map<full_opcode_type, value>;
	using ip_type = chunk::iterator_type;

	class call_frame final
	{
	public:


	public:

		explicit call_frame(function_object_raw_pointer func, ip_type ip, size_t offset)
				: function_(func), ip_(ip), stack_offset_(offset)
		{
		}

		[[nodiscard]] size_t stack_offset() const
		{
			return stack_offset_;
		}

		[[nodiscard]] function_object_raw_pointer function() const
		{
			return function_;
		}

		[[nodiscard]] ip_type& ip()
		{
			return ip_;
		}


	private:
		function_object_raw_pointer function_{};
		ip_type ip_{};
		size_t stack_offset_{};
	};

	using call_frame_list_type = std::vector<call_frame>;

public:
	virtual_machine() = delete;

	~virtual_machine();

	explicit virtual_machine(helper::console& cons,
			std::shared_ptr<object_heap> heap);

	virtual_machine_status run(function_object_raw_pointer func);

private:
	virtual_machine_status run();

	// {return status, exit}
	std::tuple<std::optional<virtual_machine_status>, bool> run_code(chunk::code_type instruction, call_frame& frame);

	template<class ...TArgs>
	void runtime_error(std::string_view fmt, const TArgs& ...args)
	{

		cons_->error() << std::format("[Line {}] in file {}:",
				top_call_frame().function()->body()->line_of(top_call_frame().ip()),
				top_call_frame().function()->body()->filename()) << std::endl;

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

	template<typename TOp>
	requires std::invocable<TOp, string_object_raw_pointer, string_object_raw_pointer>
	inline void binary_op(TOp op)
	{
		try
		{
			auto right = get_string(peek(0));
			auto left = get_string(peek(1));

			pop_two_and_push(op(left, right));
		}
		catch (const std::exception& e)
		{
			this->runtime_error("Invalid operands for binary operator: {}", e.what());
		}
	}

	void call_value(const value& val, size_t arg_count);

	void call(function_object_raw_pointer func, size_t arg_count);

	bool is_false(const value& val);

	bool is_true(const value& val)
	{
		return !is_false(val);
	}

	// stack modification
	void reset_stack();

	value peek(size_t offset = 0);

	value pop();

	void push(const value& val);

	inline void pop_two_and_push(const value& val);
	//

	// instruction reading
	value next_constant();

	std::string next_variable_name();

	chunk::code_type next_code();

	value& slot_at(const call_frame& frame, size_t slot);

	// call frame

	call_frame& top_call_frame()
	{
		return *call_frames_.rbegin();
	}

	void push_call_frame(function_object_raw_pointer func, chunk::iterator_type ip, size_t stack_offset)
	{
		call_frames_.emplace_back(func, ip, stack_offset);
	}

	void pop_call_frame()
	{
		call_frames_.pop_back();
	}

	//

	std::shared_ptr<object_heap> heap_{};

//	std::shared_ptr<chunk> chunk_{};

//	ip_type ip_{};

	value_list_type stack_{};

	global_table_type globals_{};

	function_table_type functions_{};

	call_frame_list_type call_frames_{};

	mutable helper::console* cons_{ nullptr };
};


}
