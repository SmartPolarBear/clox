
#include <helper/enum.h>

#include <resolver/lox_type.h>

#include <tuple>
#include <utility>
#include <format>

using namespace clox;

using namespace clox::helper;

using namespace clox::resolving;

uint64_t lox_any_type::flags() const
{
	return TYPE_PRIMITIVE;
}

type_id lox_any_type::id() const
{
	return PRIMITIVE_TYPE_ID_ANY;
}

bool lox_any_type::operator<(const lox_type& target) const
{
	return true;
}

std::string lox_any_type::printable_string()
{
	return "<any type>";
}

bool lox_any_type::operator==(const lox_type& t) const
{
	return true;
}

bool lox_any_type::operator!=(const lox_type& lox_type) const
{
	return false;
}
