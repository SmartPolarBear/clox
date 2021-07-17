#include "driver/driver.h"

#include <helper/std_console.h>

#include <scanner/scanner.h>

#include <parser/parser.h>
#include <parser/ast_printer.h>

#include <logger/logger.h>

#include <interpreter/interpreter.h>

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

int clox::driver::run_code(helper::console& output_cons, const string& code)
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

	interpreter the_interpreter{ output_cons };
	resolver rsv{ &the_interpreter };

	rsv.resolve(stmts);

	if (logger::instance().has_errors())return 65;
	else if (logger::instance().has_runtime_errors())return 67;

	the_interpreter.interpret(stmts);

	if (logger::instance().has_errors())return 65;
	else if (logger::instance().has_runtime_errors())return 67;

	return 0;
}

int clox::driver::run_file(helper::console& cons, const std::string& name)
{
	ifstream src{ name };

	stringstream ss{};
	ss << src.rdbuf();

	return run_code(cons, ss.str());
}

int clox::driver::run_repl(helper::console& cons)
{
	interpreter the_interpreter{ cons };
	resolver rsv{ &the_interpreter };

	cons.out() << ">>> ";

	for (auto line = cons.read_line(); line.has_value(); line = cons.read_line())
	{
		auto _ = gsl::finally([&cons]
		{
			cons.out() << ">>> ";
		});

		logger::instance().clear_error();

		scanner sc{ line.value_or("") };
		parser ps{ sc.scan() };
		auto stmt = ps.parse();

		if (logger::instance().has_errors())
			continue;

		rsv.resolve(stmt);
		if (logger::instance().has_errors() || logger::instance().has_runtime_errors())
			continue;

		the_interpreter.interpret(stmt);
		if (logger::instance().has_errors() || logger::instance().has_runtime_errors())
			continue;

	}

	return 0;
}
