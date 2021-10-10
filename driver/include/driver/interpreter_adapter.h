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
// Created by cleve on 9/10/2021.
//

#pragma once

#include <parser/gen/parser_base.inc>
#include <parser/gen/parser_classes.inc>

#include <resolver/resolver.h>
#include <interpreter/vm/vm.h>

#include <string>
#include <vector>
#include <memory>

#include <concepts>

namespace clox::driver
{
class interpreter_adapter
{
public:

	virtual int full_code(const std::vector<std::shared_ptr<parsing::statement>>& stmts) = 0;

	virtual int repl(const std::vector<std::shared_ptr<parsing::statement>>& stmts) = 0;

};

class vm_interpreter_adapter final
		: public interpreter_adapter
{
public:

	explicit vm_interpreter_adapter(helper::console& cons)
			: heap_(std::make_shared<interpreting::vm::object_heap>(cons)),
			  cons_(&cons),
			  repl_resolver_(),
			  repl_vm_(cons, heap_)
	{
	}


	int full_code(const std::vector<std::shared_ptr<parsing::statement>>& stmts) override;

	int repl(const std::vector<std::shared_ptr<parsing::statement>>& stmts) override;

private:
	std::shared_ptr<interpreting::vm::object_heap> heap_{};

	resolving::resolver repl_resolver_{};
	interpreting::vm::virtual_machine repl_vm_;

	mutable helper::console* cons_{};
};

}
