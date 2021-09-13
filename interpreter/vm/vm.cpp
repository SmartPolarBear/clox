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

#include <helper/exceptions.h>


#include <interpreter/vm/vm.h>
#include <interpreter/vm/exceptions.h>
#include <interpreter/vm/opcode.h>

#include <gsl/gsl>
#include <interpreter/vm/string_object.h>

using namespace std;
using namespace gsl;

using namespace clox::interpreting;
using namespace clox::interpreting::vm;


virtual_machine::virtual_machine(clox::helper::console& cons,
		std::shared_ptr<object_heap> heap)
		: heap_(std::move(heap)), cons_(&cons)
{
	stack_.reserve(STACK_RESERVED_SIZE);
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

	for (; ip_ != chunk_->end();)
	{
		try
		{
			chunk::code_type instruction = *ip_++;
			run_code(instruction);
		}
		catch (const vm_return& vr)
		{
			return vr.status();
		}
		catch (const exception& e)
		{
			runtime_error("{}", e.what());
			return virtual_machine_status::RUNTIME_ERROR;
		}
	}

	return virtual_machine_status::OK;
}


bool virtual_machine::run_code(chunk::code_type instruction)
{
	switch (V(main_op_code_of(instruction)))
	{
	case V(op_code::RETURN):
	{
		pop();
		throw vm_return{ virtual_machine_status::OK };
	}

	case V(op_code::POP):
	{
		pop();
		break;
	}

	case V(op_code::POP_N):
	{
		const auto count = static_cast<size_t>(next_code());
		for (size_t i = 0; i < count; i++)
		{
			pop();
		}
		break;
	}

	case V(op_code::CONSTANT):
	{
		auto constant = next_constant();
		push(constant);
		break;
	}
	case V(op_code::CONSTANT_NIL):
	{
		push(scanning::nil_value_tag);
		break;
	}
	case V(op_code::CONSTANT_TRUE):
	{
		push(true);
		break;
	}
	case V(op_code::CONSTANT_FALSE):
	{
		push(false);
		break;
	}


	case V(op_code::NEGATE):
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

	case V(op_code::NOT):
	{
		push(is_false(pop()));
		break;
	}

	case V(op_code::ADD):
	{
		if (is_string_value(peek(1)))
		{
			binary_op([this](string_object_raw_pointer lp, string_object_raw_pointer rp) -> object_raw_pointer
			{
				return heap_->allocate<string_object>(lp->string() + rp->string());
			});
		}
		else
		{
			binary_op([](scanning::floating_literal_type l, scanning::floating_literal_type r)
			{
				return l + r;
			});
		}
		break;
	}
	case V(op_code::SUBTRACT):
	{
		binary_op([](scanning::floating_literal_type l, scanning::floating_literal_type r)
		{
			return l - r;
		});
		break;
	}
	case V(op_code::MULTIPLY):
	{
		binary_op([](scanning::floating_literal_type l, scanning::floating_literal_type r)
		{
			return l * r;
		});
		break;
	}
	case V(op_code::DIVIDE):
	{
		binary_op([](scanning::floating_literal_type l, scanning::floating_literal_type r)
		{
			return l / r;
		});
		break;
	}
	case V(op_code::POW):
	{
		binary_op([](scanning::floating_literal_type l, scanning::floating_literal_type r)
		{
			return std::pow(l, r);
		});
		break;
	}

	case V(op_code::LESS):
	{
		binary_op([](scanning::floating_literal_type l, scanning::floating_literal_type r)
		{
			return l < r;
		});
		break;
	}
	case V(op_code::LESS_EQUAL):
	{
		binary_op([](scanning::floating_literal_type l, scanning::floating_literal_type r)
		{
			return l <= r;
		});
		break;
	}
	case V(op_code::GREATER):
	{
		binary_op([](scanning::floating_literal_type l, scanning::floating_literal_type r)
		{
			return l > r;
		});
		break;
	}
	case V(op_code::GREATER_EQUAL):
	{
		binary_op([](scanning::floating_literal_type l, scanning::floating_literal_type r)
		{
			return l >= r;
		});
		break;
	}
	case V(op_code::EQUAL):
	{
		auto right = peek(0);
		auto left = peek(1);

		pop_two_and_push(left == right);

		break;
	}

	case V(op_code::PRINT):
	{
		cons_->out() << visit(value_stringify_visitor{ false }, pop()) << endl;
		break;
	}

	case V(op_code::GET):
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
			push(stack_[slot]);
		}
		else
		{
			throw invalid_opcode{ instruction };
		}

		break;
	}


	case V(op_code::SET):
	{

		auto secondary = secondary_op_code_of(instruction);
		if (secondary & SEC_OP_GLOBAL)
		{
			auto name = next_variable_name();
			globals_.at(name) = peek(0);
			push(peek(0));
		}
		else if (secondary & SEC_OP_LOCAL)
		{
			auto slot = next_code();
			stack_[slot] = peek(0);
			push(stack_[slot]); // assignment expression should create a value
		}
		else
		{
			throw invalid_opcode{ instruction };
		}

		break;
	}


	case V(op_code::DEFINE):
	{
		auto secondary = secondary_op_code_of(instruction);
		if (secondary & SEC_OP_GLOBAL)
		{
			auto name = next_variable_name();
			globals_.insert_or_assign(name, peek(0));
			pop();
		}
		else // one can only define global
		{
			throw invalid_opcode{ instruction };
		}

		break;
	}


	case V(op_code::INC):
	case V(op_code::DEC):
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
			auto prev_val = stack_[slot];

			stack_[slot] = std::visit(inc_dec_visitor, prev_val);

			if (secondary & SEC_OP_POSTFIX)
			{
				push(prev_val);
			}
			else
			{
				push(stack_[slot]);
			}

		}
		else
		{
			throw invalid_opcode{ instruction };
		}

		break;
	}

	case V(op_code::JUMP):
	{
		auto offset = next_code();
		ip_ += offset;
		break;
	}

	case V(op_code::JUMP_IF_FALSE):
	{
		auto offset = next_code();
		if (is_false(peek(0)))
		{
			ip_ += offset;
		}
		break;
	}

	case V(op_code::LOOP):
	{
		auto offset = next_code();
		ip_ -= offset;
		break;
	}

	default:
		throw invalid_opcode{ instruction };
	}

	return true;
}

value virtual_machine::next_constant()
{
	return chunk_->constant_at(next_code());
}

chunk::code_type virtual_machine::next_code()
{
	return *ip_++;
}


value virtual_machine::pop()
{
	auto ret = stack_.back();
	stack_.pop_back();
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
	pop();
	pop();

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
		else if constexpr(is_same_v<T, scanning::nil_value_tag_type>)
		{
			return false;
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

virtual_machine_status virtual_machine::run(const shared_ptr<chunk>& chunk)
{
	chunk_ = chunk;
	ip_ = chunk_->begin();

	return run();
}

