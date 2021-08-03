
#include <helper/enum.h>

#include <resolver/lox_type.h>

#include <tuple>

using namespace clox;

using namespace clox::helper;

clox::resolving::lox_type_flags clox::resolving::error_type::type()
{
	return lox_type_flags::TYPE_ERROR;
}

size_t clox::resolving::error_type::size()
{
	return 0;
}

clox::resolving::lox_type_flags clox::resolving::nil_type::type()
{
	return lox_type_flags::TYPE_PRIMITIVE;
}

size_t clox::resolving::nil_type::size()
{
	return enum_cast(lox_primitive_type_size::NIL);
}

resolving::lox_type_flags resolving::integer_type::type()
{
	return lox_type_flags::TYPE_PRIMITIVE;
}

size_t resolving::integer_type::size()
{
	return enum_cast(lox_primitive_type_size::INTEGER);
}

resolving::lox_type_flags resolving::boolean_type::type()
{
	return lox_type_flags::TYPE_PRIMITIVE;
}

size_t resolving::boolean_type::size()
{
	return enum_cast(lox_primitive_type_size::BOOLEAN);
}

resolving::lox_type_flags resolving::floating_type::type()
{
	return resolving::lox_type_flags::TYPE_PRIMITIVE;
}

size_t resolving::floating_type::size()
{
	return enum_cast(lox_primitive_type_size::FLOATING);
}
