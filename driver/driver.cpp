#include <base/configuration.h>

#include <driver/driver.h>
#include <driver/interpreter_adapter.h>
#include "driver/adapter/classic.h"
#include "driver/adapter/vm.h"

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

using namespace clox::base;
using namespace clox::helper;
using namespace clox::scanning;
using namespace clox::parsing;
using namespace clox::logging;
using namespace clox::resolving;
using namespace clox::interpreting;

int clox::driver::run(const argparse::ArgumentParser& arg_parser)
{
	auto file = arg_parser.get<string>("--file");

	configurable_configuration_instance().load_arguments(arg_parser);

	shared_ptr<interpreter_adapter> adapter{ nullptr };
	if (arg_parser.get<bool>("--classic"))
	{
		if (configurable_configuration_instance().dump_assembly())
		{
			logger::instance().error("--show-ast", "In classic mode, there is no assembly code to display.");
			return 1;
		}

		adapter = static_pointer_cast<interpreter_adapter>(
				make_shared<classic_interpreter_adapter>(clox::helper::std_console::instance()));

	}
	else
	{
		adapter = static_pointer_cast<interpreter_adapter>(
				make_shared<vm_interpreter_adapter>(clox::helper::std_console::instance()));
	}


	if (!file.empty())
	{
		return clox::driver::run_file(clox::helper::std_console::instance(),
				adapter,
				file);
	}
	else
	{
		return clox::driver::run_repl(clox::helper::std_console::instance(),
				adapter);
	}

	return 0;
}
