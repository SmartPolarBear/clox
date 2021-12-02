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
#include <driver/driver.h>
#include <driver/interpreter_adapter.h>

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


using namespace std;
using namespace gsl;

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

	classic::interpreter the_interpreter{ *cons_ };

	rsv.resolve(stmts);

	if (logger::instance().has_errors())return 65;
	else if (logger::instance().has_runtime_errors())return 67;

	codegen gen{ heap_, rsv };

	virtual_machine vm{ *cons_, heap_ };

	garbage_collector gc{ *cons_, heap_, vm, gen };

	heap_->enable_gc(gc);

	auto _ = finally([this]
	{
		heap_->remove_gc();
	});

	gen.generate(stmts);

	if (logger::instance().has_errors())return 65;
	else if (logger::instance().has_runtime_errors())return 67;

	if (configurable_configuration_instance().dump_assembly())
	{
		gen.top_level()->function()->body()->disassemble(*cons_);
	}

	vm.run(gen.top_level());

	if (logger::instance().has_errors())return 65;
	else if (logger::instance().has_runtime_errors())return 67;

	return 0;
}

int clox::driver::vm_interpreter_adapter::repl(const std::vector<std::shared_ptr<parsing::statement>>& stmts)
{
	repl_resolver_.resolve(stmts);

	if (logger::instance().has_errors())return 65;
	else if (logger::instance().has_runtime_errors())return 67;

	codegen gen{ heap_, repl_resolver_ };

	garbage_collector gc{ *cons_, heap_, repl_vm_, gen };

	heap_->enable_gc(gc);

	auto _ = finally([this]
	{
		heap_->remove_gc();
	});

	gen.generate(stmts);

	if (configurable_configuration_instance().dump_assembly())
	{
		gen.top_level()->function()->body()->disassemble(*cons_);
	}

	if (logger::instance().has_errors())return 65;
	else if (logger::instance().has_runtime_errors())return 67;

	repl_vm_.run(gen.top_level());  // FIXME

	if (logger::instance().has_errors())return 65;
	else if (logger::instance().has_runtime_errors())return 67;

	return 0;
}


