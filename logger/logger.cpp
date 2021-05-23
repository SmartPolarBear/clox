#include <logger.h>

#include <iostream>
#include <format>

using namespace std;

void clox::logger::logger::error(int line, std::string message)
{
	cout << format("[Line {}] Error: {}", line, message) << endl;
}
