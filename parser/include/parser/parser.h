#pragma once

#include "parser_classes.inc"

namespace clox::parsing
{

class parser final
{
public:
	[[nodiscard]] explicit parser(std::vector<scanning::token>&& tokens)
			: tokens_{ tokens }
	{
	}

private:
	std::vector<scanning::token> tokens_;

	size_t cur_{ 0 };
};

}

