#include <logger/logger.h>

#include <iostream>
#include <format>

using namespace std;

void clox::logger::logger::error(size_t line, const std::string& message)
{
	errors_++;
	cout << format("[Line {}] Error: {}", line, message) << endl;
}
