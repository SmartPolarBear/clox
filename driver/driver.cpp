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


//#define AST_PRINTING

using namespace std;

using namespace clox::scanning;
using namespace clox::parsing;
using namespace clox::logging;
using namespace clox::interpreting;

static inline int run(string code)
{
	scanner sc{ std::move(code) };
	parser ps{ sc.scan() };

	auto expr = ps.parse();

	if (logger::instance().has_errors())return 65;
	else if (logger::instance().has_runtime_errors())return 67;

#ifdef AST_PRINTING
	ast_printer pt{};
	cout << pt.to_string(*expr) << endl;
#else
	interpreter::instance().interpret(expr);
#endif

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
