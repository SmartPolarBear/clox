#include <iostream>
#include <format>

#include <helper/std_console.h>

#include <argparse/argparse.hpp>

#include "driver/driver.h"

using namespace std;

int main(int argc, char *argv[])
{
	argparse::ArgumentParser arg_parser{"clox"};

	arg_parser.add_argument("-f", "--file")
		.help("the script to run_code")
		.default_value(string{""})
		.implicit_value(string{""})
		.nargs(1);

	arg_parser.add_argument("-a", "--classic")
		.help("Use classic (tree-walker) interpreter instead of bytecode virtual machine")
		.default_value(false)
		.implicit_value(true);

	arg_parser.add_argument("-t", "--show-ast")
		.help("Show AST structure")
		.default_value(false)
		.implicit_value(true);

	arg_parser.add_argument("-d", "--show-assembly")
		.help("Show assembly code")
		.default_value(false)
		.implicit_value(true);

	arg_parser.add_argument("-vd", "--verbose-debug")
		.help("Verbose debug output.")
		.default_value(false)
		.implicit_value(true);

	arg_parser.add_argument("-t", "--time-statistic")
		.help("Show time statistic like the runtime, compile time, etc.")
		.default_value(false)
		.implicit_value(true);

	try
	{
		arg_parser.parse_args(argc, argv);
	}
	catch (const std::runtime_error &err)
	{
		cout << err.what() << endl;
		cout << arg_parser;
		return 1;
	}

	try
	{
		return clox::driver::run(arg_parser);
	}
	catch (const std::exception &e)
	{
		cerr << e.what() << endl;
		return 1;
	}
}
