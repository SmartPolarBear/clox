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

#include <base/configuration.h>

#include <driver/classic.h>
#include "driver/driver.h"

#include <helper/std_console.h>

#include <scanner/scanner.h>

#include <parser/parser.h>
#include <parser/ast_printer.h>

#include <logger/logger.h>

#include <interpreter/vm/chunk.h>

#include <resolver/resolver.h>

#include <interpreter/codegen/codegen.h>

#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>

#include <gsl/gsl>
#include <driver/interpreter_adapter.h>


using namespace std;

using namespace clox::base;
using namespace clox::helper;
using namespace clox::scanning;
using namespace clox::parsing;
using namespace clox::logging;
using namespace clox::resolving;
using namespace clox::interpreting;
using namespace clox::interpreting::vm;
using namespace clox::interpreting::compiling;


int clox::driver::vm_interpreter_adapter::full_code(const std::vector<std::shared_ptr<parsing::statement>>& stmts)
{
	resolver rsv{};

	classic::interpreter the_interpreter{ *cons_, rsv.bindings() };

	rsv.resolve(stmts);

	if (logger::instance().has_errors())return 65;
	else if (logger::instance().has_runtime_errors())return 67;

	codegen gen_{ heap_, rsv.bindings() };
	gen_.generate(stmts);

	if (logger::instance().has_errors())return 65;
	else if (logger::instance().has_runtime_errors())return 67;

	if (configurable_configuration_instance().dump_assembly())
	{
		gen_.top_function()->body()->disassemble(*cons_);
	}

	virtual_machine vm{ *cons_, heap_ };
	vm.run(gen_.top_function());

	if (logger::instance().has_errors())return 65;
	else if (logger::instance().has_runtime_errors())return 67;

	return 0;
}

int clox::driver::vm_interpreter_adapter::repl(const std::vector<std::shared_ptr<parsing::statement>>& stmts)
{
	repl_resolver_.resolve(stmts);

	if (logger::instance().has_errors())return 65;
	else if (logger::instance().has_runtime_errors())return 67;

	codegen gen_{ heap_, repl_resolver_.bindings() };
	gen_.generate(stmts);

	if (configurable_configuration_instance().dump_assembly())
	{
		gen_.top_function()->body()->disassemble(*cons_);
	}

	if (logger::instance().has_errors())return 65;
	else if (logger::instance().has_runtime_errors())return 67;

	repl_vm_.run(gen_.top_function());  // FIXME


	if (logger::instance().has_errors())return 65;
	else if (logger::instance().has_runtime_errors())return 67;

	return 0;
}


