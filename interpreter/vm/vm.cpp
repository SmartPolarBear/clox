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

#include <interpreter/vm/vm.h>
#include <interpreter/vm/exceptions.h>
#include <interpreter/vm/opcode.h>

#include <gsl/gsl>

#ifdef V
#warning "V is already defined"
#undef V
#endif

using namespace std;
using namespace gsl;

using namespace clox::interpreting;
using namespace clox::interpreting::vm;

#define V(op) op_code_value(op)

clox::interpreting::vm::virtual_machine_status clox::interpreting::vm::virtual_machine::run()
{
	for (; ip_ != chunk_->end();)
	{
		switch (chunk::code_type instruction = *ip_++;instruction)
		{
		case V(op_code::RETURN):
		{
			pop();
			return virtual_machine_status::OK;
		}
		case V(op_code::CONSTANT):
		{
			auto constant = next_constant();
			push(constant);
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

		default:
			throw invalid_opcode{ instruction };
		}
	}

	// should not reach here
	return virtual_machine_status::RUNTIME_ERROR;
}

value virtual_machine::next_constant()
{
	return chunk_->constant_at(next_byte());
}

chunk::code_type virtual_machine::next_byte()
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

value virtual_machine::peek()
{
	return stack_.back();
}
