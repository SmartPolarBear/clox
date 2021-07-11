#pragma once

#include <base.h>

#include <helper/console.h>
#include <scanner/scanner.h>
#include <interpreter/runtime_error.h>

#include <iostream>
#include <string>

namespace clox::logging
{
class logger final
		: public base::singleton<logger>
{
public:
	logger();

	~logger() = default;

	logger(const logger&) = delete;

	logger(logger&&) = delete;

	logger& operator=(const logger&) = delete;

	void set_console(helper::console&);

	helper::console& get_console();

	void error(size_t line, const std::string& message);

	void error(const scanning::token& token, const std::string& msg);

	void runtime_error(const interpreting::runtime_error& re);

	[[nodiscard]] bool has_errors() const;

	[[nodiscard]] bool has_runtime_errors() const;

private:

	size_t errors_{ 0 };
	size_t runtime_errors_{ 0 };

	mutable helper::console* console_{ nullptr };
};
}