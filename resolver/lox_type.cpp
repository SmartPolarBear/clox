
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

uint64_t clox::resolving::nil_type::flags()
{
	return lox_type_flags::TYPE_PRIMITIVE;
}

size_t clox::resolving::nil_type::size()
{
	return enum_cast(lox_primitive_type_size::NIL);
}

uint64_t resolving::integer_type::flags()
{
	return lox_type_flags::TYPE_PRIMITIVE;
}

size_t resolving::integer_type::size()
{
	return enum_cast(lox_primitive_type_size::INTEGER);
}

uint64_t resolving::boolean_type::flags()
{
	return lox_type_flags::TYPE_PRIMITIVE;
}

size_t resolving::boolean_type::size()
{
	return enum_cast(lox_primitive_type_size::BOOLEAN);
}

uint64_t resolving::floating_type::flags()
{
	return resolving::lox_type_flags::TYPE_PRIMITIVE;
}

size_t resolving::floating_type::size()
{
	return enum_cast(lox_primitive_type_size::FLOATING);
}
