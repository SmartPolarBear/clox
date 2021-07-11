#include <logger/logger.h>

#include <scanner/scanner.h>
#include <helper/std_console.h>

#include <iostream>
#include <format>

using namespace std;

using namespace clox::scanning;

clox::logging::logger::logger()
		: console_(&helper::std_console::instance())
{
}

void clox::logging::logger::set_console(clox::helper::console& cons)
{
	console_ = &cons;
}


void clox::logging::logger::error(size_t line, const std::string& message)
{
	errors_++;
	console_->stream() << format("[Line {}] Error: {}", line, message) << endl;
}

void clox::logging::logger::error(const clox::scanning::token& token, const std::string& msg)
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

bool clox::logging::logger::has_errors() const
{
	return errors_;
}

void clox::logging::logger::runtime_error(const clox::interpreting::runtime_error& re)
{
	runtime_errors_++;
	console_->stream() << std::format("[Line {1}, at runtime] {0}\n", re.what(), re.token().line()) << endl;
}

bool clox::logging::logger::has_runtime_errors() const
{
	return runtime_errors_;
}

clox::helper::console& clox::logging::logger::get_console()
{
	return *console_;
}

