#pragma once

#include <scanner/scanner.h>
#include <scanner/nil_value.h>

#include <parser/gen/parser_base.inc>
#include <parser/parser_base.h>

#include <variant>

namespace clox::parsing
{


/// \brief base class for all expressions defined in parser_classes.inc
class expression : public parser_class_base
{
};

/// \brief base class for type_expression
class type_expression : public expression
{

};

}
