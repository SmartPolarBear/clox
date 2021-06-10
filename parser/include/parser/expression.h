#pragma once

#include "scanner/scanner.h"

namespace clox::parsing
{
/// \brief base class for all expressions defined in parser_classes.inc
template<typename T>
class expression
{
public:
	T accept()
};
}
