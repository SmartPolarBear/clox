#include <logger/logger.h>

#include <scanner/scanner.h>

#include <iostream>
#include <format>

using namespace std;

using namespace clox::scanning;

void clox::logger::logger::error(size_t line, const std::string& message)
{
	errors_++;
	cout << format("[Line {}] Error: {}", line, message) << endl;
}

void clox::logger::logger::error(clox::scanning::token token, std::string msg)
{
	if (token.type() == token_type::FEND)
	{
		error(token.line(), std::format("{} at end.\n", msg));
	}
	else
	{
		error(token.line(), std::format(" at '{0}': {1}\n", token.lexeme(), msg));
	}
}
