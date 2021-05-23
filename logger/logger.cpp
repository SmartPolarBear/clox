#include <logger.h>

#include <iostream>
#include <format>

using namespace std;

void clox::logger::logger::error(int line, const std::string& message)
{
	errors_++;
	cout << format("[Line {}] Error: {}", line, message) << endl;
}
