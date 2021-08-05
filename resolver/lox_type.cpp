
#include <helper/enum.h>

#include <resolver/lox_type.h>

#include <tuple>

using namespace clox;

using namespace clox::helper;

uint64_t clox::resolving::error_type::flags()
{
	return lox_type_flags::TYPE_ERROR | lox_type_flags::TYPE_PRIMITIVE;
}

size_t clox::resolving::error_type::size()
{
	return 0;
}

std::string resolving::error_type::printable_string()
{
	return "<error type>";
}

uint64_t clox::resolving::nil_type::flags()
{
	return lox_type_flags::TYPE_PRIMITIVE;
}

size_t clox::resolving::nil_type::size()
{
	return enum_cast(lox_primitive_type_size::NIL);
}

std::string resolving::nil_type::printable_string()
{
	return "nil";
}

uint64_t resolving::integer_type::flags()
{
	return lox_type_flags::TYPE_PRIMITIVE;
}

size_t resolving::integer_type::size()
{
	return enum_cast(lox_primitive_type_size::INTEGER);
}

std::string resolving::integer_type::printable_string()
{
	return "integer";
}

uint64_t resolving::boolean_type::flags()
{
	return lox_type_flags::TYPE_PRIMITIVE;
}

size_t resolving::boolean_type::size()
{
	return enum_cast(lox_primitive_type_size::BOOLEAN);
}

std::string resolving::boolean_type::printable_string()
{
	return "boolean";
}

uint64_t resolving::floating_type::flags()
{
	return resolving::lox_type_flags::TYPE_PRIMITIVE;
}

size_t resolving::floating_type::size()
{
	return enum_cast(lox_primitive_type_size::FLOATING);
}

std::string resolving::floating_type::printable_string()
{
	return "floating";
}
