#include "driver/driver.h"

#include <scanner/scanner.h>

#include <parser/parser.h>

#include <logger/logger.h>

#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>


using namespace std;

using namespace clox::scanning;
using namespace clox::parsing;
using namespace clox::logging;

static inline int run(string code)
{
	scanner sc{ std::move(code) };
	parser ps{ sc.scan() };

	auto expr = ps.parse();

	if (logger::instance().has_errors())return 1;

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
		auto e = run(line);
		if (e != 0)return e;

		cout << ">>> ";
	}

	return 0;
}
