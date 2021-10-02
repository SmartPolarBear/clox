
#include <helper/enum.h>

#include <resolver/lox_type.h>
#include <resolver/instance_type.h>

#include <tuple>
#include <utility>
#include <format>

using namespace clox;

using namespace clox::helper;

using namespace clox::resolving;


bool lox_type::unify(const lox_type& base, const lox_type& derived)
{
	if (lox_type::is_instance(base))
	{
		return unify(*dynamic_cast<const lox_instance_type&>(base).underlying_type(), derived);
	}
	if (lox_type::is_instance(derived))
	{
		return unify(base, *dynamic_cast<const lox_instance_type&>(derived).underlying_type());
	}

	if (base.id() == PRIMITIVE_TYPE_ID_ANY)
	{
		return true;
	}

	if (base.id() == PRIMITIVE_TYPE_ID_VOID)
	{
		return false;
	}

//	if (base.id() == TYPE_ID_INITIALIZER_LIST)
//	{
//		return false;
//	}

	return derived < base;

//	if (derived.id() == TYPE_ID_INITIALIZER_LIST)
//	{
//		return dynamic_cast<const initializer_list_type&>(derived) < dynamic_cast<const lox_object_type&>(base);
//	}
//
//
//	return dynamic_cast<const lox_object_type&>(derived) < dynamic_cast<const lox_object_type&>(base);
}

std::shared_ptr<lox_type> lox_type::intersect(const std::shared_ptr<lox_type>& t1, const std::shared_ptr<lox_type>& t2)
{
	if (t1->id() == PRIMITIVE_TYPE_ID_ANY || t2->id() == PRIMITIVE_TYPE_ID_ANY)return std::make_shared<lox_any_type>();

	if (is_primitive(*t1) && is_primitive(*t2))
	{
		return t1->id() >= t2->id() ? t1 : t2;
	}

	if (*t1 < *t2)return t2;

	if (*t2 < *t1)return t1;

	auto o1 = std::static_pointer_cast<lox_object_type>(t1), o2 = std::static_pointer_cast<lox_object_type>(t2);
	if (o1->depth() < o2->depth())std::swap(o1, o2);

	while (o1->depth() <= o1->depth())
	{
		o1 = o1->super();
	}

	while (o1 && o2 && *o1 != *o2)
	{
		o1 = o1->super();
		o2 = o2->super();
	}

	if (o1 == nullptr || o2 == nullptr)
	{
		return nullptr;
	}

	return o1;
}

