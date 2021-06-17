#pragma once

#include <base.h>

#include "scanner/scanner.h"

#include <interpreter/runtime_error.h>

#include <string>

namespace clox::logging
{
class logger final
		: public base::singleton<logger>
{
public:
	void error(size_t line, const std::string& message);

	void error(scanning::token token, std::string msg);

	void runtime_error(const interpreting::runtime_error &re);

	[[nodiscard]] bool has_errors() const;

	[[nodiscard]] bool has_runtime_errors() const;

private:
	size_t errors_{ 0 };
	size_t runtime_errors_{ 0 };
};
}