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

#include <base/iterable_stack.h>

#include <interpreter/vm/opcode.h>
#include <interpreter/vm/chunk.h>
#include <interpreter/vm/value.h>
#include <interpreter/vm/exceptions.h>

#include <interpreter/vm/heap.h>
#include <interpreter/vm/heap_allocator.h>
#include <interpreter/vm/garbage_collector.h>


#include <interpreter/vm/string_object.h>
#include <interpreter/vm/closure_object.h>

#include <memory>
#include <map>
#include <ranges>

#include <gsl/gsl>

namespace clox::interpreting::vm
{

enum class virtual_machine_status
{
	OK,
	RUNTIME_ERROR,
};

template<typename F, typename R, class ...Args>
concept BinaryOperator=
requires(F&& f, Args&& ... args) {
	{ std::invoke(std::forward<F>(f), std::forward<Args>(args)...) }->std::same_as<R>;
};

class virtual_machine final
{
public:
	friend class garbage_collector;

	static inline constexpr size_t CALL_STACK_RESERVED_SIZE = 64;
	static inline constexpr size_t STACK_RESERVED_SIZE = 16384;

	using value_list_type = std::vector<value>;
	using global_table_type = std::unordered_map<std::string, value>;
	using function_table_type = std::unordered_map<full_opcode_type, value>;
	using ip_type = chunk::iterator_type;

	using index_type = gsl::index;

	class call_frame final
	{
	public:


	public:

		explicit call_frame(closure_object_raw_pointer closure, ip_type ip, size_t offset)
				: closure_(closure), ip_(ip), stack_offset_(offset)
		{
		}

		[[nodiscard]] size_t stack_offset() const
		{
			return stack_offset_;
		}

		[[nodiscard]] function_object_raw_pointer function() const
		{
			return closure_->function();
		}

		[[nodiscard]] closure_object_raw_pointer closure() const
		{
			return closure_;
		}

		[[nodiscard]] ip_type& ip()
		{
			return ip_;
		}


	private:
		closure_object_raw_pointer closure_{};
		ip_type ip_{};
		size_t stack_offset_{};
	};

	using call_frame_list_type = std::vector<call_frame>;

public:
	virtual_machine() = delete;

	~virtual_machine();

	explicit virtual_machine(helper::console& cons,
			std::shared_ptr<object_heap> heap);

	virtual_machine_status run(clox::interpreting::vm::closure_object* closure);

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

		cons_->error() << "Call stack:" << std::endl;
		for (auto& frm: call_frames_ | std::ranges::views::reverse)
		{
			auto func = frm.function();
			auto line = func->body()->line_of(frm.ip() - 1);
			cons_->error() << std::format("[Line {}] in ", line);
			if (func->name().empty())
			{
				cons_->error() << "script" << std::endl;
			}
			else
			{
				cons_->error() << func->name() << std::endl;
			}
		}

		reset_stack();
	}

	template<typename TOp>
	requires BinaryOperator<TOp, floating_value_type, floating_value_type, floating_value_type>
	inline void binary_op(TOp op)
	{
		try
		{
			auto l = peek(0), r = peek(1);

			auto right = get_number_promoted(l);

			auto left = get_number_promoted(r);

			auto ret = op(left, right);

			if (std::holds_alternative<floating_value_type>(l) ||
				std::holds_alternative<floating_value_type>(r))
			{
				pop_two_and_push(ret);
			}
			else if (std::holds_alternative<integer_value_type>(l) ||
					 std::holds_alternative<integer_value_type>(r))
			{
				pop_two_and_push(static_cast<integer_value_type>(ret));
			}
			else if (std::holds_alternative<boolean_value_type>(l) ||
					 std::holds_alternative<boolean_value_type>(r))
			{
				pop_two_and_push(static_cast<scanning::boolean_literal_type>(ret));
			}
			else
			{
				pop_two_and_push(
						static_cast<integer_value_type>(ret)); // cannot combine for the sake of the rules of type promoting
			}
		}
		catch (const std::exception& e)
		{
			this->runtime_error("Invalid operands for binary operator: {}", e.what());
			throw e;
		}
	}

	template<typename TOp>
	requires BinaryOperator<TOp, object_raw_pointer, string_object_raw_pointer, string_object_raw_pointer>
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
			throw e;
		}
	}

	// for comparing, whose op returns bool value
	template<typename TOp>
	requires BinaryOperator<TOp, bool, floating_value_type, floating_value_type>
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
			throw e;
		}
	}

	template<typename TOp>
	requires BinaryOperator<TOp, bool, string_object_raw_pointer, string_object_raw_pointer>
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
			throw e;
		}
	}


	void call_value(const value& val, size_t arg_count);

	void call(closure_object_raw_pointer closure, size_t arg_count);

	bool is_false(const value& val);

	bool is_true(const value& val)
	{
		return !is_false(val);
	}

	// stack modification
	void reset_stack();


	template<object_pointer T>
	T peek_object(size_t offset = 0)
	{
		return std::visit([](auto&& val) -> T
		{
			using TV = std::decay_t<decltype(val)>;
			if constexpr (std::is_same_v<TV, object_raw_pointer>)
			{
				if (auto func = dynamic_cast<T>(val);func)
				{
					return func;
				}
				else
				{
					throw invalid_value{ val };
				}
			}
			else
			{
				throw invalid_value{ val };
			}
		}, peek(offset));
	}

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

	upvalue_object_raw_pointer capture_upvalue(value* val, index_type stack_index);

	void close_upvalues(index_type last);

	// call frame

	call_frame& top_call_frame()
	{
		return *call_frames_.rbegin();
	}

	void push_call_frame(closure_object_raw_pointer closure, chunk::iterator_type ip, size_t stack_offset)
	{
		call_frames_.emplace_back(closure, ip, stack_offset);
	}

	void pop_call_frame()
	{
		call_frames_.pop_back();
	}

	//

	std::shared_ptr<object_heap> heap_{};

	garbage_collector gc_;

	value_list_type stack_{};

	global_table_type globals_{};

	function_table_type functions_{};

	call_frame_list_type call_frames_{};

	std::map<index_type, upvalue_object_raw_pointer> open_upvalues_{}; // it should be ordered

	mutable helper::console* cons_{ nullptr };
};


}
