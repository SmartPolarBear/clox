#include <iostream>
#include <format>

#include <argparse/argparse.hpp>

#include "driver.h"

using namespace std;

int main(int argc, char* argv[])
{
	argparse::ArgumentParser prog{ "clox" };

	prog.add_argument("f", "--file")
			.help("the script to run")
			.default_value("")
			.implicit_value("");

	try
	{
		prog.parse_args(argc, argv);
	}
	catch (const std::runtime_error& err)
	{
		cout << err.what() << endl;
		cout << prog;
		return 1;
	}

	auto file=prog.get<string>("--file");
	if(file.empty())
	{
		return clox::driver::run_file(file);
	}
	else
	{
		return clox::driver::run_command();
	}

	return 0;
}
