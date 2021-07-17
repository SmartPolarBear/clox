#include <iostream>
#include <format>

#include <helper/std_console.h>

#include <argparse/argparse.hpp>

#include "driver/driver.h"

using namespace std;

int main(int argc, char* argv[])
{
	argparse::ArgumentParser arg_parser{ "clox" };

	arg_parser.add_argument("-f", "--file")
			.help("the script to run_code")
			.default_value(string{ "" })
			.implicit_value(string{ "" })
			.nargs(1);

	try
	{
		arg_parser.parse_args(argc, argv);
	}
	catch (const std::runtime_error& err)
	{
		cout << err.what() << endl;
		cout << arg_parser;
		return 1;
	}

	auto file = arg_parser.get<string>("--file");
	if (!file.empty())
	{
		return clox::driver::run_file(clox::helper::std_console::instance(), file);
	}
	else
	{
		return clox::driver::run_repl(clox::helper::std_console::instance());
	}
}
