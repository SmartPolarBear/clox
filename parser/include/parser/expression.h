#pragma once

#include <scanner/scanner.h>


namespace clox::parsing
{


/// \brief base class for all expressions defined in parser_classes.inc
class expression
{
public:

};

template<typename T>
class visitable
{
	template<typename R>
	R accept(visitor <R>& vis)
	{
		dynamic_cast<T>(this)->template accept<R>(vis);
	}
};
}
