#include "driver/driver.h"

#include <scanner/scanner.h>

#include <parser/parser.h>
#include <parser/ast_printer.h>

#include <logger/logger.h>

#include <interpreter/interpreter.h>

#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>


using namespace std;

using namespace clox::scanning;
using namespace clox::parsing;
using namespace clox::logging;
using namespace clox::interpreting;

static inline int run(string code)
{
	scanner sc{ std::move(code) };
	parser ps{ sc.scan() };

	auto stmts = ps.parse();

	if (logger::instance().has_errors())return 65;
	else if (logger::instance().has_runtime_errors())return 67;


	interpreter::instance().interpret(std::move(stmts));

	return 0;
}

int clox::driver::run_file(const std::string& name)
{
	ifstream src{ name };
	stringstream ss{};
	ss << src.rdbuf();


	return run(ss.str());
}

int clox::driver::run_command()
{
	std::string line;

	cout << ">>> ";
	while (std::getline(std::cin, line))
	{
		auto _ = run(line);

		cout << ">>> ";
	}

	return 0;
}
