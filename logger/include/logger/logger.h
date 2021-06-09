#pragma once

#include <base.h>

#include "scanner/scanner.h"

#include <string>

namespace clox::logger
{
class logger final
		: public base::singleton<logger>
{
public:
	void error(size_t line, const std::string& message);

	void error(scanning::token token, std::string msg);

private:
	size_t errors_{ 0 };
};
}