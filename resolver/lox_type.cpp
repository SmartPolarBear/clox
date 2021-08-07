
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


std::shared_ptr<lox_object_type> resolving::lox_object_type::super() const
{
	return super_.lock();
}

uint64_t lox_object_type::depth() const
{
	return depth_;
}

bool resolving::lox_object_type::operator<(const resolving::lox_type& another) const
{
	if (!dynamic_cast<const lox_object_type*>(&another))
		return false;

	const auto& obj = dynamic_cast<const lox_object_type&>(another);

	return *this < obj;
}

bool lox_object_type::operator<(const lox_object_type& obj) const
{
	if (is_primitive(*this) && is_primitive(obj))
	{
		return this->id() <= obj.id(); // this is a hack
	}

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

	return false;
}

lox_object_type::lox_object_type(std::string name, type_id id, uint64_t flags,
		const std::shared_ptr<lox_object_type>& parent)
		: name_(std::move(name)),
		  id_(id),
		  flags_(TYPE_CLASS | flags),
		  super_(parent),
		  depth_(parent ? parent->depth() + 1 : 0)
{
}

std::string lox_object_type::printable_string()
{
	return std::format("<class {}>", name_);
}

uint64_t lox_object_type::flags() const
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
		inst = std::make_shared<lox_object_type>("object", PRIMITIVE_TYPE_ID_OBJECT, TYPE_PRIMITIVE, nullptr);
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
		: lox_object_type("integer", PRIMITIVE_TYPE_ID_INTEGER, TYPE_PRIMITIVE, lox_object_type::object())
{
}


lox_floating_type::lox_floating_type()
		: lox_object_type("floating", PRIMITIVE_TYPE_ID_FLOATING, TYPE_PRIMITIVE, lox_object_type::object())
{
}


lox_boolean_type::lox_boolean_type()
		: lox_object_type("boolean", PRIMITIVE_TYPE_ID_BOOLEAN, TYPE_PRIMITIVE, lox_object_type::object())
{
}


lox_nil_type::lox_nil_type()
		: lox_object_type("nil", PRIMITIVE_TYPE_ID_NIL, TYPE_PRIMITIVE, lox_object_type::object())
{
}


bool lox_type::unify(const lox_type& base, const lox_type& derived)
{
	return derived.id() == PRIMITIVE_TYPE_ID_ANY ||
		   dynamic_cast<const lox_object_type&>(derived) < dynamic_cast<const lox_object_type&>(base);
}
