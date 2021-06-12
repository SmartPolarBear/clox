#pragma once

#include <scanner/scanner.h>

#include <parser/parser_classes.inc>
#include <parser/parser_base.inc>

namespace clox::parsing
{


/// \brief base class for all expressions defined in parser_classes.inc
class expression
{
public:
	virtual parser_class_type get_type() const = 0;
};

template<typename T>
class visitable
{
	template<typename R>
	R accept(visitor<R>& vis)
	{
		dynamic_cast<T>(this)->template accept<R>(vis);
	}
};
}
