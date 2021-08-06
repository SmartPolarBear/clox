
#include <helper/enum.h>

#include <resolver/lox_type.h>

#include <tuple>
#include <utility>
#include <format>

using namespace clox;

using namespace clox::helper;

using namespace clox::resolving;

uint64_t lox_any_type::flags()
{
	return TYPE_PRIMITIVE;
}

type_id lox_any_type::id() const
{
	return PRIMITIVE_TYPE_ID_ANY;
}

bool lox_any_type::operator<(const lox_type& target)
{
	return true;
}

std::string lox_any_type::printable_string()
{
	return "<any type>";
}


std::shared_ptr<lox_object_type> resolving::lox_object_type::super() const
{
	return super_.lock();
}

uint64_t lox_object_type::depth() const
{
	return depth_;
}

bool resolving::lox_object_type::operator<(const resolving::lox_type& another)
{
	if (!dynamic_cast<const lox_object_type*>(&another))return false;

	const auto& obj = dynamic_cast<const lox_object_type&>(another);

	return *this < obj;
}

bool lox_object_type::operator<(const lox_object_type& obj)
{
	if (this->depth() < obj.depth())
	{
		return false;
	}
	else if (this->depth() == obj.depth())
	{
		return this->id() == obj.id();
	}

	auto pa = this->super();
	for (; pa && pa->depth() <= obj.depth(); pa = pa->super())
	{
		if (pa->id() == obj.id())
		{
			return true;
		}
	}

	auto another_pa = obj.super();
	for (; pa && another_pa; pa = pa->super(), another_pa = another_pa->super())
	{
		if (pa->id() == another_pa->id())
		{
			return true;
		}
	}

	return false;
}

lox_object_type::lox_object_type(std::string name, type_id id, const std::shared_ptr<lox_object_type>& parent)
		: name_(std::move(name)),
		  id_(id),
		  flags_(TYPE_CLASS),
		  super_(parent),
		  depth_(parent ? parent->depth() + 1 : 0)
{
}

std::string lox_object_type::printable_string()
{
	return std::format("<class {}>", name_);
}

uint64_t lox_object_type::flags()
{
	return flags_;
}

type_id lox_object_type::id() const
{
	return id_;
}


std::vector<std::shared_ptr<lox_object_type>>& lox_object_type::derived()
{
	return derived_;
}

std::shared_ptr<lox_object_type> lox_object_type::object()
{
	static std::shared_ptr<lox_object_type> inst{ nullptr };
	if (!inst)
	{
		inst = std::make_shared<lox_object_type>("object", PRIMITIVE_TYPE_ID_OBJECT, nullptr);
	}
	return inst;
}

std::shared_ptr<lox_object_type> lox_object_type::integer()
{
	static std::shared_ptr<lox_object_type> inst{ nullptr };
	if (!inst)
	{
		inst = std::make_shared<lox_integer_type>();
		object()->derived_.push_back(inst);
	}

	return inst;
}

std::shared_ptr<lox_object_type> lox_object_type::floating()
{
	static std::shared_ptr<lox_object_type> inst{ nullptr };
	if (!inst)
	{
		inst = std::make_shared<lox_floating_type>();
		object()->derived_.push_back(inst);
	}
	return inst;
}

std::shared_ptr<lox_object_type> lox_object_type::boolean()
{
	static std::shared_ptr<lox_object_type> inst{ nullptr };
	if (!inst)
	{
		inst = std::make_shared<lox_boolean_type>();
		object()->derived_.push_back(inst);
	}
	return inst;
}

std::shared_ptr<lox_object_type> lox_object_type::nil()
{
	static std::shared_ptr<lox_object_type> inst{ nullptr };
	if (!inst)
	{
		inst = std::make_shared<lox_nil_type>();
		object()->derived_.push_back(inst);
	}
	return inst;
}


lox_integer_type::lox_integer_type()
		: lox_object_type("integer", PRIMITIVE_TYPE_ID_INTEGER, lox_object_type::object())
{
}

bool lox_integer_type::operator<(const lox_object_type& obj)
{
	if (dynamic_cast<const lox_floating_type*>(&obj))
	{
		return true;
	}

	return lox_object_type::operator<(obj);
}

lox_floating_type::lox_floating_type()
		: lox_object_type("floating", PRIMITIVE_TYPE_ID_FLOATING, lox_object_type::object())
{
}

bool lox_floating_type::operator<(const lox_object_type& obj)
{
	return lox_object_type::operator<(obj);
}

lox_boolean_type::lox_boolean_type()
		: lox_object_type("boolean", PRIMITIVE_TYPE_ID_BOOLEAN, lox_object_type::object())
{
}

bool lox_boolean_type::operator<(const lox_object_type& obj)
{
	if (dynamic_cast<const lox_floating_type*>(&obj) ||
		dynamic_cast<const lox_integer_type*>(&obj))
	{
		return true;
	}
	return lox_object_type::operator<(obj);
}

lox_nil_type::lox_nil_type()
		: lox_object_type("nil", PRIMITIVE_TYPE_ID_NIL, lox_object_type::object())
{
}

bool lox_nil_type::operator<(const lox_object_type& obj)
{
	return lox_object_type::operator<(obj);
}
