// Copyright (c) 2021 SmartPolarBear
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//
// Created by cleve on 8/14/2021.
//

#include <resolver/resolver.h>

#include <resolver/lox_type.h>
#include <resolver/object_type.h>
#include <resolver/callable_type.h>
#include <resolver/instance_type.h>
#include <resolver/initializer_list_type.h>

#include <logger/logger.h>

#include <memory>
#include <format>
#include <ranges>
#include <tuple>

#include <gsl/gsl>

using namespace clox::parsing;
using namespace clox::logging;
using namespace clox::resolving;
using namespace clox::interpreting;

using namespace std;
using namespace gsl;


std::shared_ptr<lox_type>
resolver::visit_assignment_expression(const std::shared_ptr<parsing::assignment_expression>& e)
{
	auto value_type = resolve(e->get_value());

	// set the depth of expression
	auto symbol = resolve_local(e, e->get_name());
	if (symbol->symbol_type() != symbol_type::ST_NAMED)
	{
		return type_error(e->get_name(), std::format("{} is not a variable", e->get_name().lexeme()));
	}

	auto compa = check_type_assignment(e->get_name(), static_pointer_cast<named_symbol>(symbol)->type(), value_type);

	return get<0>(compa);
}

std::shared_ptr<lox_type> resolver::visit_binary_expression(const std::shared_ptr<parsing::binary_expression>& expr)
{
	auto l_type = resolve(expr->get_left());
	auto r_type = resolve(expr->get_right());

	auto ret = check_type_binary_expression(expr->get_op(), l_type, r_type);

	if (auto operator_binding_ret = get<2>(ret);operator_binding_ret)
	{
		auto[stmt, method]=operator_binding_ret.value();

		auto get_expr = make_shared<get_expression>(expr->get_left(), expr->get_op());

		scanning::token virtual_paren{ scanning::virtual_token };

		auto call_expr = make_shared<call_expression>(get_expr, virtual_paren,
				vector<shared_ptr<expression>>{ expr->get_right() });

		bindings_->put<operator_binding>(expr, expr, call_expr);
		bindings_->put<function_binding>(call_expr, call_expr, stmt);

	}

	return get<0>(ret);
}

std::shared_ptr<lox_type> resolver::visit_unary_expression(const std::shared_ptr<parsing::unary_expression>& ue)
{
	auto type = resolve(ue->get_right());

	auto ret = check_type_unary_expression(ue->get_op(), type);
	return get<0>(ret);
}

std::shared_ptr<lox_type> resolver::visit_postfix_expression(const std::shared_ptr<parsing::postfix_expression>& pe)
{
	auto type = resolve(pe->get_left());
	auto ret = check_type_postfix_expression(pe->get_op(), type);

	return get<0>(ret);
}


std::shared_ptr<lox_type> resolver::visit_literal_expression(const std::shared_ptr<parsing::literal_expression>& le)
{
	if (holds_alternative<long long>(le->get_value()))
	{
		return lox_object_type::integer();
	}
	else if (holds_alternative<long double>(le->get_value()))
	{
		return lox_object_type::floating();
	}
	else if (holds_alternative<bool>(le->get_value()))
	{
		return lox_object_type::boolean();
	}
	else if (holds_alternative<scanning::nil_value_tag_type>(le->get_value()))
	{
		return lox_object_type::nil();
	}
	else if (holds_alternative<std::string>(le->get_value()))
	{
		return lox_object_type::string();
	}
	else
	{
		return type_error(le->get_token(), "Invalid literal value of unknown type.");
	}
}

std::shared_ptr<lox_type> resolver::visit_grouping_expression(const std::shared_ptr<parsing::grouping_expression>& ge)
{
	return resolve(ge->get_expr());
}

std::shared_ptr<lox_type> resolver::visit_var_expression(const std::shared_ptr<parsing::var_expression>& ve)
{
	if (!scopes_.empty())
	{
		if (auto existence = scope_top_find(ve->get_name().lexeme());existence.has_value() &&
																	 !existence.value())
		{
			logger::instance().error(ve->get_name(),
					std::format("Reading {} before its initialization.", ve->get_name().lexeme()));
		}
	}

	auto symbol = resolve_local(ve, ve->get_name());

	if (!symbol)
	{
		return type_error(ve->get_name(), std::format("Name \"{}\" is not exist.", ve->get_name().lexeme()));
	}

	return symbol->type();
}

std::shared_ptr<lox_type> resolver::visit_ternary_expression(const std::shared_ptr<parsing::ternary_expression>& te)
{
	auto cond_type = resolve(te->get_cond());
	auto t_type = resolve(te->get_true_expr());
	auto f_type = resolve(te->get_false_expr());

	if (!lox_type::unify(*lox_object_type::boolean(), *cond_type))
	{
		return type_error(te->get_qmark(), "The condition for ternary operator should be compatible with boolean.");
	}

	auto value_type_ret = check_type_ternary_expression(te->get_colon(), t_type, f_type);

	return get<0>(value_type_ret);
}

std::shared_ptr<lox_type> resolver::visit_logical_expression(const std::shared_ptr<parsing::logical_expression>& le)
{
	auto l_type = resolve(le->get_left());
	auto r_type = resolve(le->get_right());

	auto ret = check_type_logical_expression(le->get_op(), l_type, r_type);

	return get<0>(ret);
}


std::shared_ptr<lox_type> resolver::visit_get_expression(const std::shared_ptr<get_expression>& ptr)
{
	auto obj_type = resolve(ptr->get_object());

	if (!lox_type::is_instance(*obj_type))
	{
		return type_error(ptr->get_name(), std::format("{} is not a instance", obj_type->printable_string()));
	}

	auto inst = static_pointer_cast<lox_instance_type>(obj_type);

	if (!lox_type::is_class(*inst->underlying_type()))
	{
		return type_error(ptr->get_name(), std::format("{} of type {} is not a class type", ptr->get_name().lexeme(),
				inst->underlying_type()->printable_string()));
	}

	auto class_type = static_pointer_cast<lox_class_type>(inst->underlying_type());
	auto member_name = ptr->get_name().lexeme();

	for (class_type; class_type; class_type = dynamic_pointer_cast<lox_class_type>(class_type->super()))
	{
		if (class_type->fields().contains(member_name))
		{
			return class_type->fields().at(member_name);
		}
		else if (class_type->methods().contains(member_name))
		{
			return class_type->methods().at(member_name);
		}
	}

	return type_error(ptr->get_name(), std::format("Instance of type {} do not have a member named {}",
			class_type->printable_string(), member_name));
}

std::shared_ptr<lox_type> resolver::visit_set_expression(const std::shared_ptr<set_expression>& se)
{
	auto value_type = resolve(se->get_val());

	auto object_type = resolve(se->get_object());
	if (lox_type::is_instance(*object_type))
	{
		object_type = static_pointer_cast<lox_instance_type>(object_type)->underlying_type();
	}

	if (!lox_type::is_class(*object_type))
	{
		return type_error(se->get_name(), std::format("Set property {} of non-class type {}", se->get_name().lexeme(),
				object_type->printable_string()));
	}

	auto class_type = static_pointer_cast<lox_class_type>(object_type);

	auto property_type = class_type->fields()[se->get_name().lexeme()];

	auto ret = check_type_assignment(se->get_name(), property_type, value_type);

	return get<0>(ret);
}

std::shared_ptr<lox_type> resolver::visit_this_expression(const std::shared_ptr<this_expression>& expr)
{

	if (cur_class_.top() == env_class_type::CT_NONE)
	{
		logger::instance().error(expr->get_keyword(), "Can't use this in standalone function or in global scoop.");
		return make_shared<lox_any_type>();
	}

	auto symbol = resolve_local(expr, expr->get_keyword());

	return symbol->type();
}

std::shared_ptr<lox_type> resolver::visit_base_expression(const std::shared_ptr<base_expression>& be)
{
	if (cur_class_.top() == env_class_type::CT_NONE)
	{
		logger::instance().error(be->get_keyword(), "Can't use super in standalone function or in global scoop.");
		return make_shared<lox_any_type>();
	}
	else if (cur_class_.top() != env_class_type::CT_INHERITED_CLASS)
	{
		logger::instance().error(be->get_keyword(), "Can't use super in class who doesn't have a base class.");
		return make_shared<lox_any_type>();
	}

	auto symbol = resolve_local(be, be->get_keyword());

	if (!lox_type::is_class(*symbol->type()))
	{
		throw logic_error{ "base should have a class type" };
	}

	auto class_type = static_pointer_cast<lox_class_type>(symbol->type());

	if (class_type->methods().contains(be->get_member().lexeme()))
	{
		return class_type->methods().at(be->get_member().lexeme());
	}
	else if (class_type->fields().contains(be->get_member().lexeme()))
	{
		return class_type->fields().at(be->get_member().lexeme());
	}

	return type_error(be->get_member(),
			std::format("Base class do not have a member named {}", be->get_member().lexeme()));
}


std::shared_ptr<lox_type> resolver::visit_call_expression(const std::shared_ptr<parsing::call_expression>& ce)
{
	auto callee = resolve(ce->get_callee());

	if (!callee)
	{
		throw logic_error{ "callee isn't nullable" };
	}

	vector<shared_ptr<lox_type>> args{};
	for (const auto& arg: ce->get_args())
	{
		auto type = resolve(arg);
		args.push_back(type);
	}

	auto[callable_type, compatible]=check_type_call_expression(ce, callee, args);

	if (!compatible)
	{
		return callable_type;
	}

	auto callable = dynamic_pointer_cast<lox_callable_type>(callable_type);

	if (!callable->return_type_deduced())
	{
		return type_error(ce->get_paren(), "Too complex type for deducing. Specify return type explicitly !");
	}

	auto return_type = callable->return_type();

	if (lox_type::is_class(*return_type))
	{
		return make_shared<lox_instance_type>(static_pointer_cast<lox_class_type>(return_type));
	}

	return return_type;
}

shared_ptr<lox_type>
resolver::visit_initializer_list_expression(const std::shared_ptr<initializer_list_expression>& ile)
{
	vector<shared_ptr<lox_type>> items{};
	for (const auto& item: ile->get_items())
	{
		items.push_back(resolve(item));
	}
	return make_shared<initializer_list_type>(items);
}


