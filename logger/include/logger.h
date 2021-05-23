#pragma once

#include <base.h>

#include <string>

namespace clox::logger
{
class logger final
		: public base::singleton<logger>
{
public:
	void error(int line,const std::string& message);

private:
	size_t errors_{0};
};
}