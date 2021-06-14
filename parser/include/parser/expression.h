#pragma once

#include <scanner/scanner.h>

#include <parser/gen/parser_base.inc>
#include <scanner/nil_value.h>

#include <variant>


namespace clox::parsing
{


/// \brief base class for all expressions defined in parser_classes.inc
class expression
{
public:
	[[nodiscard]] virtual parser_class_type get_type() const
	{
		return parser_class_type::PC_TYPE_invalid;
	};
};


}
