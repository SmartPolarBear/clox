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

#include <base/configuration.h>
#include <helper/exceptions.h>

#include <interpreter/vm/vm.h>
#include <interpreter/vm/exceptions.h>
#include <interpreter/vm/opcode.h>

#include <interpreter/vm/string_object.h>
#include <interpreter/vm/closure_object.h>
#include <interpreter/vm/upvalue_object.h>
#include <interpreter/vm/instance_object.h>

#include <gsl/gsl>
#include "interpreter/vm/class_object.h"
#include "interpreter/vm/bounded_method_object.h"


using namespace std;
using namespace gsl;

using namespace clox::base;
using namespace clox::interpreting;
using namespace clox::interpreting::vm;


virtual_machine::virtual_machine(clox::helper::console& cons,
		std::shared_ptr<object_heap> heap)
		: heap_(std::move(heap)), cons_(&cons)
{
	stack_.reserve(STACK_RESERVED_SIZE);
	call_frames_.reserve(CALL_STACK_RESERVED_SIZE);
}

virtual_machine::~virtual_machine()
{
}

void virtual_machine::reset_stack()
{
	stack_.clear();
	stack_.shrink_to_fit();

	stack_.reserve(STACK_RESERVED_SIZE);
}

clox::interpreting::vm::virtual_machine_status clox::interpreting::vm::virtual_machine::run()
{


//	for (; ip_ != chunk_->end();)
	for (; top_call_frame().ip() != top_call_frame().function()->body()->end();)
	{
//		try
//		{
		chunk::code_type instruction = *top_call_frame().ip()++;
		auto[status, exit] = run_code(instruction, top_call_frame());
		if (exit)
		{
			return status.value_or(virtual_machine_status::OK);
		}
//		}
//		catch (const exception& e)
//		{
//			runtime_error("{}", e.what());
//			return virtual_machine_status::RUNTIME_ERROR;
//		}
	}

	return virtual_machine_status::OK;
}


std::tuple<std::optional<virtual_machine_status>, bool>
virtual_machine::run_code(chunk::code_type instruction, call_frame& frame)
{
	switch (main_op_code_of(instruction))
	{
		using enum op_code;

	case RETURN:
	{
		auto ret = pop();

		while (stack_.size() > top_call_frame().stack_offset()) // pop function's value
		{
			pop();
		}

		if (call_frames_.empty())
		{
			pop();
			return { virtual_machine_status::OK, true };
		}

		close_upvalues(frame.stack_offset());

		push(ret);
		pop_call_frame();
		break;
	}

	case PUSH:
	{
		auto secondary = secondary_op_code_of(instruction);

		if (secondary & SEC_OP_FUNC)
		{
			auto id = next_code();
			push(functions_.at(id));
		}
		else if (secondary & SEC_OP_CLASS)
		{
			auto name = next_variable_name();
			push(globals_.at(name));
		}
		else
		{
			throw invalid_opcode{ instruction };
		}

		break;
	}

	case POP:
	{
		pop();
		break;
	}

	case POP_N:
	{
		const auto count = static_cast<size_t>(next_code());
		for (size_t i = 0; i < count; i++)
		{
			pop();
		}
		break;
	}

	case CONSTANT:
	{
		auto constant = next_constant();
		push(constant);
		break;
	}
	case CONSTANT_NIL:
	{
		push(scanning::nil_value_tag);
		break;
	}
	case CONSTANT_TRUE:
	{
		push(true);
		break;
	}
	case CONSTANT_FALSE:
	{
		push(false);
		break;
	}


	case NEGATE:
	{
		push(std::visit([](auto&& val) -> value
		{
			using T = std::decay_t<decltype(val)>;
			if constexpr(std::is_same_v<T, scanning::floating_literal_type> ||
						 std::is_same_v<T, scanning::integer_literal_type>)
			{
				return -val;
			}

			throw invalid_value{ val };
		}, pop()));
		break;
	}

	case NOT:
	{
		push(is_false(pop()));
		break;
	}

	case ADD:
	{

		if (is_string_value(peek(1)) || is_string_value(peek(1)))
		{
			binary_op([this](string_object_raw_pointer lp, string_object_raw_pointer rp) -> object_raw_pointer
			{
				return string_object::create_on_heap(heap_, lp->string() + rp->string());
			});
		}
		else
		{
			binary_op([](floating_value_type l, floating_value_type r) -> floating_value_type
			{
				return l + r;
			});
		}


		break;
	}
	case SUBTRACT:
	{
		binary_op([](floating_value_type l, floating_value_type r) -> floating_value_type
		{
			return l - r;
		});
		break;
	}
	case MULTIPLY:
	{
		binary_op([](floating_value_type l, floating_value_type r) -> floating_value_type
		{
			return l * r;
		});
		break;
	}
	case DIVIDE:
	{
		binary_op([](floating_value_type l, floating_value_type r) -> floating_value_type
		{
			return l / r;
		});
		break;
	}
	case POW:
	{
		binary_op([](floating_value_type l, floating_value_type r) -> floating_value_type
		{
			return std::pow(l, r);
		});
		break;
	}

	case LESS:
	{
		binary_op([](scanning::floating_literal_type l, scanning::floating_literal_type r) -> bool
		{
			return l < r;
		});
		break;
	}
	case LESS_EQUAL:
	{
		binary_op([](scanning::floating_literal_type l, scanning::floating_literal_type r) -> bool
		{
			return l <= r;
		});
		break;
	}
	case GREATER:
	{
		binary_op([](scanning::floating_literal_type l, scanning::floating_literal_type r) -> bool
		{
			return l > r;
		});
		break;
	}
	case GREATER_EQUAL:
	{
		binary_op([](scanning::floating_literal_type l, scanning::floating_literal_type r) -> bool
		{
			return l >= r;
		});
		break;
	}
	case EQUAL:
	{
		if (is_string_value(peek(1)) || is_string_value(peek(1)))
		{
			binary_op([](string_object_raw_pointer lp, string_object_raw_pointer rp) -> bool
			{
				return lp->string() == rp->string();
			});
		}
		else
		{
			binary_op([](scanning::floating_literal_type l, scanning::floating_literal_type r) -> bool
			{
				return l == r;
			});
		}


		break;
	}

	case PRINT:
	{
		cons_->out() << visit(value_stringify_visitor{ false }, pop()) << endl;
		break;
	}

	case GET:
	{
		auto secondary = secondary_op_code_of(instruction);
		if (secondary & SEC_OP_GLOBAL)
		{
			auto name = next_variable_name();
			push(globals_.at(name));
		}
		else if (secondary & SEC_OP_LOCAL)
		{
			auto slot = next_code();
			push(slot_at(frame, slot));
		}
		else if (secondary & SEC_OP_UPVALUE)
		{
			auto slot = next_code();
			auto val = *frame.closure()->upvalues()[slot]->get_value();
			push(val);
		}
		else
		{
			throw invalid_opcode{ instruction };
		}

		break;
	}


	case SET:
	{

		auto secondary = secondary_op_code_of(instruction);
		if (secondary & SEC_OP_GLOBAL)
		{
			auto name = next_variable_name();
			globals_.at(name) = peek(0);
		}
		else if (secondary & SEC_OP_LOCAL)
		{
			auto slot = next_code();
			slot_at(frame, slot) = peek(0);
		}
		else if (secondary & SEC_OP_UPVALUE)
		{
			auto slot = next_code();
			*frame.closure()->upvalues()[slot]->get_value() = peek();
		}
		else
		{
			throw invalid_opcode{ instruction };
		}

		// Do not push it because it already at the top of the stack

		break;
	}


	case DEFINE:
	{
		auto secondary = secondary_op_code_of(instruction);
		if (secondary & SEC_OP_GLOBAL)
		{
			auto name = next_variable_name();
			globals_.insert_or_assign(name, peek(0));
			pop();
		}
		else if (secondary & SEC_OP_FUNC)
		{
			auto id = next_code();
			auto func_obj = next_constant();

			functions_.insert_or_assign(id, func_obj);
		}
		else // one can only define global
		{
			throw invalid_opcode{ instruction };
		}

		break;
	}

		// FIXME: fix the bug that CLOSE_UPVALUE is generated after return so that it will never be executed
	case CLOSE_UPVALUE:
	{
		close_upvalues(stack_.size() - 1);
		pop();
		break;
	}

	case INC:
	case DEC:
	{
		auto secondary = secondary_op_code_of(instruction);

		static const auto inc_dec_visitor = [&](auto&& val) -> value
		{
			using T = std::decay_t<decltype(val)>;
			if constexpr(std::is_same_v<T, integer_value_type>)
			{
				if (main_op_code_of(instruction) == op_code::INC)
				{
					return val + 1;
				}
				else if (main_op_code_of(instruction) == op_code::DEC)
				{
					return val - 1;
				}
				else
				{
					throw invalid_opcode(instruction);
				}
			}
			else if constexpr(std::is_same_v<T, floating_value_type>)
			{
				if (main_op_code_of(instruction) == op_code::INC)
				{
					return val + 1.0;
				}
				else if (main_op_code_of(instruction) == op_code::DEC)
				{
					return val - 1.0;
				}
				else
				{
					throw invalid_opcode(instruction);
				}
			}
			else
			{
				throw invalid_value{ val };
			}
		};

		if (secondary & SEC_OP_GLOBAL)
		{
			auto name = next_variable_name();
			auto prev_val = globals_.at(name);
			globals_.at(name) = std::visit(inc_dec_visitor, prev_val);

			if (secondary & SEC_OP_POSTFIX)
			{
				push(prev_val);
			}
			else
			{
				push(globals_.at(name));
			}
		}
		else if (secondary & SEC_OP_LOCAL)
		{
			auto slot = next_code();

			auto prev_val = slot_at(frame, slot);

			slot_at(frame, slot) = std::visit(inc_dec_visitor, prev_val);

			if (secondary & SEC_OP_POSTFIX)
			{
				push(prev_val);
			}
			else
			{
				push(slot_at(frame, slot));
			}

		}
		else
		{
			throw invalid_opcode{ instruction };
		}

		break;
	}

	case JUMP:
	{
		auto offset = next_code();
		frame.ip() += offset;
		break;
	}

	case JUMP_IF_FALSE:
	{
		auto offset = next_code();
		if (is_false(peek(0)))
		{
			frame.ip() += offset;
		}
		break;
	}

	case LOOP:
	{
		auto offset = next_code();
		frame.ip() -= offset;
		break;
	}

	case CLOSURE:
	{
		auto func = peek_object<function_object_raw_pointer>();

		auto closure = func->wrapper_closure();
		if (!closure)
		{
			closure = heap_->allocate<closure_object>(func);
		}

		pop();
		push(closure);

		auto secondary = secondary_op_code_of(instruction);

		if (secondary & SEC_OP_CAPTURE)
		{
			auto count = next_code();
			for (int i = 0; i < count; i++)
			{
				auto local = next_code();
				auto index = next_code();
				if (local)
				{
					closure->upvalues().push_back(
							capture_upvalue(&slot_at(frame, index), frame.stack_offset() + index));
				}
				else
				{
					closure->upvalues().push_back(frame.closure()->upvalues()[index]);
				}
			}
		}

		break;
	}

	case CALL:
	{
		auto arg_count = next_code();

		auto closure = peek(arg_count);

		call_value(closure, arg_count);

		break;
	}

	case CLASS:
	{
		auto name = next_variable_name();
		auto fields_size = next_code();

		push(heap_->allocate<class_object>(name, fields_size));
		break;
	}

	case INSTANCE:
	{
		auto class_obj = peek_object<class_object_raw_pointer>();
		// TODO: constructor may have arguments ?

		auto secondary = secondary_op_code_of(instruction);
		if (secondary & SEC_OP_FUNC) // will call a user-defined constructor
		{
			auto id = next_code();
			auto args = next_code();

			pop();
			auto instance = heap_->allocate<instance_object>(class_obj);

			push(instance);

			push(instance);

			if (!bind_method(instance, id))
			{
				return { virtual_machine_status::RUNTIME_ERROR, true };
			}

			auto bound = peek();
			call_value(bound, args);
		}
		else
		{
			pop();
			push(heap_->allocate<instance_object>(class_obj));
		}

		break;
	}

	case SET_PROPERTY:
	{
		auto offset = next_code();
		auto cls = peek_object<instance_object_raw_pointer>(1);
		cls->set(offset, peek());
		auto val = pop();
		pop(); // remove the class instance from stack
		push(val);
		break;
	}

	case GET_PROPERTY:
	{
		auto secondary = secondary_op_code_of(instruction);

		auto cls = peek_object<instance_object_raw_pointer>();

		if (secondary & SEC_OP_FUNC) [[unlikely]]
		{
			auto id = next_code();
			if (!bind_method(cls, id))
			{
				return { virtual_machine_status::RUNTIME_ERROR, true };
			}
		}
		else [[likely]]
		{
			auto offset = next_code();
			auto val = cls->get(offset);
			pop(); // discard the instance
			push(val);
		}
		break;
	}

	case METHOD:
	{
		auto id = next_code();

		auto method_closure = peek_object<function_object_raw_pointer>(0)->wrapper_closure();
		auto class_obj = peek_object<class_object_raw_pointer>(1);

		pop();

		class_obj->put_method(id, method_closure);
		break;
	}

	default:
		throw invalid_opcode{ instruction };
	}

	return { nullopt, false };
}


value& virtual_machine::slot_at(const call_frame& frame, size_t slot)
{
	return *(stack_.begin() + frame.stack_offset() + slot);
}

value virtual_machine::next_constant()
{
//	return chunk_->constant_at(next_code());
	return top_call_frame().function()->body()->constant_at(next_code());
}

chunk::code_type virtual_machine::next_code()
{
	return *top_call_frame().ip()++;
}


value virtual_machine::pop()
{
	auto ret = stack_.back();
	stack_.pop_back();
	assert(!stack_.empty());
	return ret;
}

void virtual_machine::push(const value& val)
{
	stack_.push_back(val);
}

value virtual_machine::peek(size_t offset)
{
	return *(stack_.rbegin() + offset);
}

inline void virtual_machine::pop_two_and_push(const value& val)
{
	auto left = pop();
	auto right = pop();

	push(val);
}

bool virtual_machine::is_false(const value& val)
{
	return std::visit([this](auto&& val) -> bool
	{
		using T = std::decay_t<decltype(val)>;

		if constexpr(is_same_v<T, bool>)
		{
			return !static_cast<scanning::boolean_literal_type>(val);
		}
		else if constexpr(is_same_v<T, nil_value_type>)
		{
			return true; // the value is false, so is_false should return true
		}
		else
		{
			return false;
		}
	}, val);
}

std::string virtual_machine::next_variable_name()
{
	return get<std::string>(next_constant());
}

virtual_machine_status virtual_machine::run(closure_object_raw_pointer closure)
{
	push(closure);

	push_call_frame(closure, closure->function()->body()->begin(), 0);

	return run();
}

void virtual_machine::call_value(const value& val, size_t arg_count)
{
	if (!holds_alternative<object_raw_pointer>(val))
	{
		throw invalid_value{ val };
	}

	auto obj = get<object_raw_pointer>(val);

	if (obj->type() == object_type::CLOSURE)[[likely]]
	{
		auto closure = dynamic_cast<closure_object_raw_pointer>(obj);
		call(closure, arg_count);
	}
	else if (obj->type() == object_type::BOUNDED_METHOD)[[likely]]
	{
		auto bound = dynamic_cast<bounded_method_object_raw_pointer>(obj);
		call(bound->method(), arg_count);
	}
	else [[unlikely]]
	{
		throw invalid_value{ val };
	}
}

void virtual_machine::call(closure_object_raw_pointer closure, size_t arg_count)
{
	if (configurable_configuration_instance().dump_assembly())
	{
		closure->function()->body()->disassemble(*cons_);
	}

	int64_t stack_offset = static_cast<int64_t>(stack_.size()) - arg_count - 1;
	assert(stack_offset >= 0);

	push_call_frame(closure,
			closure->function()->body()->begin(),
			stack_offset);
}

upvalue_object_raw_pointer virtual_machine::capture_upvalue(value* val, index_type stack_index)
{
	if (open_upvalues_.contains(stack_index))
	{
		return open_upvalues_.at(stack_index);
	}
	auto ret = heap_->allocate<upvalue_object>(val);
	open_upvalues_.insert_or_assign(stack_index, ret);
	return ret;
}

void virtual_machine::close_upvalues(index_type last)
{
	const auto begin = open_upvalues_.lower_bound(last);
	for (auto iter = begin; iter != open_upvalues_.end();)
	{
		iter->second->close();
		iter = open_upvalues_.erase(iter);
	}
}

bool virtual_machine::bind_method(instance_object_raw_pointer inst, resolving::function_id_type method)
{
	if (!inst->class_object()->contains_method(method))
	{
		runtime_error("Cannot bind method for class {} and ID {}\n", inst->class_object()->printable_string(), method);
		return false;
	}

	auto bound = heap_->allocate<bounded_method_object>(inst, inst->class_object()->method_at(method));
	pop();
	push(bound);

	// FIXME: constructor returns nil, and here the instance is pop, so now the variable becomes nil

	return true;
}

