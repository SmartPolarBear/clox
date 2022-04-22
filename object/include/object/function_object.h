// Copyright (c) 2022 SmartPolarBear
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
// Created by cleve on 9/14/2021.
//

#pragma once


#include "scanner/scanner.h"

#include "object.h"

#include <variant>
#include <string>

#include <memory>
#include <map>

namespace clox::interpreting::compiling // to avoid header circular dependency
{
class codegen;
}

namespace clox::interpreting::vm
{
class function_object final
		: public object
{
public:
	friend class closure_object;

	friend class compiling::codegen;

	explicit function_object(std::string name, size_t arity);

	[[nodiscard]] object_type type() const noexcept override;

	[[nodiscard]] std::string name() const
	{
		return name_;
	}

	[[nodiscard]] size_t arity() const
	{
		return arity_;
	}

protected:
	void blacken(struct garbage_collector* gc_inst) override;

public:

	[[nodiscard]] auto body() const
	{
		return body_;
	}

	[[nodiscard]] auto wrapper_closure() const
	{
		return wrapper_closure_;
	}

	[[nodiscard]] size_t upvalue_count() const
	{
		return upvalue_count_;
	}

	std::string printable_string() override;

private:
	std::string name_{};
	size_t arity_{};
	size_t upvalue_count_{};
	std::shared_ptr<class chunk /* to avoid header circular dependency*/ > body_{};

	class closure_object* wrapper_closure_{ nullptr };
};

using function_object_raw_pointer = function_object*;


}
