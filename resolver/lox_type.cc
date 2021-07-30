
#include <resolver/lox_type.h>
#include <tuple>

clox::resolving::lox_type::lox_type_type clox::resolving::lox_type_number::type()
{
	return TYPE_PRIMITIVE;
}

std::tuple<bool, bool, bool> clox::resolving::lox_type_number::compatible_with(const lox_type& another)
{
	return std::tuple<bool, bool, bool>();
}
