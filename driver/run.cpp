#include <driver/driver.h>
#include <driver/interpreter_adapter.h>
#include <driver/classic.h>

#include <helper/std_console.h>

#include <scanner/scanner.h>

#include <parser/parser.h>
#include <parser/ast_printer.h>

#include <logger/logger.h>

#include <interpreter/classic/interpreter.h>
#include <interpreter/vm/chunk.h>

#include <resolver/resolver.h>

#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>

#include <gsl/gsl>

using namespace std;

using namespace clox::helper;
using namespace clox::scanning;
using namespace clox::parsing;
using namespace clox::logging;
using namespace clox::resolving;
using namespace clox::interpreting;

int clox::driver::run_code(helper::console& output_cons,
		const std::shared_ptr<interpreter_adapter>& adapter,
		const string& code,
		bool dump_ast,
		bool dump_assembly)
{
	// switch to the desirable console for logging
	auto& prev_cons = logger::instance().get_console();
	auto _ = gsl::finally([&prev_cons]()
	{
		logger::instance().set_console(prev_cons);
	});

	logger::instance().set_console(output_cons);

	scanner sc{ code };
	parser ps{ sc.scan() };

	auto stmts = ps.parse();
	if (logger::instance().has_errors())return 65;

	return adapter->full_code(stmts);
}

int clox::driver::run_file(helper::console& cons, const std::shared_ptr<interpreter_adapter>& adapter,
		const std::string& name)
{
	ifstream src{ name };

	stringstream ss{};
	ss << src.rdbuf();

	return run_code(cons, adapter, ss.str());
}

int clox::driver::run_repl(helper::console& cons, const std::shared_ptr<interpreter_adapter>& adapter)
{
	resolver rsv{};
	classic::interpreter the_interpreter{ cons, rsv.bindings() };

//	auto ck = make_shared<vm::chunk>("test");
//	auto idx = ck->add_constant(244.0f);
//	ck->write(vm::op_code_value(vm::op_code::CONSTANT), 123);
//	ck->write(idx, 123);
//	ck->write(vm::op_code_value(vm::op_code::RETURN), 123);
//	ck->disassemble(cons);
//
//	while (true);

	cons.out() << ">>>";


	for (auto line = cons.read_line(); line.has_value(); line = cons.read_line())
	{
		auto _ = gsl::finally([&cons]
		{
			cons.out() << ">>>";
		});

		logger::instance().clear_error();

		scanner sc{ line.value_or("") };
		parser ps{ sc.scan() };
		auto stmt = ps.parse();


		if (logger::instance().has_errors())
			continue;

		adapter->repl(stmt);

		if (logger::instance().has_errors() || logger::instance().has_runtime_errors())
			continue;

	}

	return 0;
}
