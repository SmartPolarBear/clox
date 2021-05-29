//
// Created by cleve on 5/27/2021.
//

#include "parser.h"
#include "scanner.h"

#include <vector>

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