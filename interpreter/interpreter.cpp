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
// Created by cleve on 6/14/2021.
//

#include <scanner/scanner.h>

#include <interpreter/interpreter.h>
#include <interpreter/runtime_error.h>
#include <interpreter/native_functions.h>
#include <interpreter/lox_function.h>
#include <interpreter/lox_class.h>
#include <interpreter/lox_instance.h>
#include <interpreter/return.h>

#include <logger/logger.h>

#include <iostream>
#include <format>
#include <utility>

#include <gsl/gsl>

using namespace std;
using namespace clox::resolving;
using namespace clox::scanning;
using namespace clox::parsing;
using namespace clox::interpreting;


interpreter::interpreter(helper::console& cons, shared_ptr<binding_table> table)
		: expression_visitor<evaluating_result>(),
		  statement_visitor<void>(),
		  globals_(std::make_shared<environment>()),
		  console_(&cons),
		  locals_(std::move(table))
{
	environment_ = globals_;
	install_native_functions();
}


clox::interpreting::evaluating_result
clox::interpreting::interpreter::visit_binary_expression(const std::shared_ptr<binary_expression>& be)
{
	if (locals_->contains(be))
	{
		if (auto op_binding_ret = locals_->get(be);op_binding_ret)
		{

			if (auto op_binding = op_binding_ret.value();op_binding->type() !=
														 resolving::binding_type::BINDING_OPERATOR)
			{
				throw clox::interpreting::runtime_error(be->get_op(),
						std::format("Internal compiler error: wrong binding type."));
			}
			else
			{
				auto binding = static_pointer_cast<operator_binding>(op_binding);
				return evaluate(binding->operator_implementation_call());
			}
		}
		throw clox::interpreting::runtime_error(be->get_op(),
				std::format("Internal compiler error: wrong binding."));
	}

	auto left = evaluate(be->get_left());
	auto right = evaluate(be->get_right());

	switch (be->get_op().type())
	{
	case scanning::token_type::GREATER:
		return get_number(left) > get_number(right);
	case scanning::token_type::GREATER_EQUAL:
		return get_number(left) >= get_number(right);
	case scanning::token_type::LESS:
		return get_number(left) < get_number(right);
	case scanning::token_type::LESS_EQUAL:
		return get_number(left) <= get_number(right);

	case scanning::token_type::MINUS:
		return get_number(left) - get_number(right);
	case scanning::token_type::PLUS:
		if (is_number(left) && is_number(right))
		{
			return get_number(left) + get_number(right);
		}
		else if (holds_alternative<string>(left) && holds_alternative<string>(right))
		{
			return get<string>(left) + get<string>(right);
		}

		throw clox::interpreting::runtime_error(be->get_op(), "Operands should be strings or numbers.");
	case scanning::token_type::SLASH:
		return get_number(left) / get_number(right);
	case scanning::token_type::STAR:
		return get_number(left) * get_number(right);


	case scanning::token_type::BANG_EQUAL:
		return !is_equal(be->get_op(), left, right);
	case scanning::token_type::EQUAL_EQUAL:
		return is_equal(be->get_op(), left, right);

	case scanning::token_type::COMMA:
		return right;

	default:
		// should not reach here
		throw clox::interpreting::runtime_error(be->get_op(),
				std::format("{} is not a valid binary operator.", be->get_op().lexeme()));
		break;
	}

	// Should not reach here
	return nil_value_tag;
}

clox::interpreting::evaluating_result
clox::interpreting::interpreter::visit_unary_expression(const std::shared_ptr<unary_expression>& ue)
{
	auto right = evaluate(ue->get_right());
	auto op = ue->get_op();

	switch (op.type())
	{
	case scanning::token_type::MINUS:
		return -get_number(right);
	case scanning::token_type::BANG:
		return !is_truthy(right);
	case scanning::token_type::PLUS_PLUS:
		if (ue->get_right()->get_type() != parsing::PC_TYPE_var_expression)
		{
			throw clox::interpreting::runtime_error(op, "++ applies only to variables");
		}

		{
			auto value = get_number(right);
			auto expr = static_pointer_cast<var_expression>(ue->get_right());
			environment_->assign(expr->get_name(), value + 1);
			return value + 1;
		}

		break;
	case scanning::token_type::MINUS_MINUS:

		if (ue->get_right()->get_type() != parsing::PC_TYPE_var_expression)
		{
			throw clox::interpreting::runtime_error(ue->get_op(), "-- applies only to variables");
		}

		{
			auto value = get_number(right);
			auto expr = static_pointer_cast<var_expression>(ue->get_right());
			environment_->assign(expr->get_name(), value - 1);
			return value - 1;
		}

		break;
	default:
		//TODO: ERROR?
		break;
	}

	// Should not reach here
	return nil_value_tag;
}


evaluating_result interpreter::visit_postfix_expression(const shared_ptr<parsing::postfix_expression>& pe)
{
	auto left = evaluate(pe->get_left());
	auto op = pe->get_op();

	switch (op.type())
	{

	case scanning::token_type::PLUS_PLUS:
		if (pe->get_left()->get_type() != parsing::PC_TYPE_var_expression)
		{
			throw clox::interpreting::runtime_error(pe->get_op(), "Postfix ++ applies only to variables");
		}

		{
			auto value = get_number(left);
			auto expr = static_pointer_cast<var_expression>(pe->get_left());
			environment_->assign(expr->get_name(), value + 1);
			return value;
		}


	case scanning::token_type::MINUS_MINUS:

		if (pe->get_left()->get_type() != parsing::PC_TYPE_var_expression)
		{
			throw clox::interpreting::runtime_error(pe->get_op(), "Postfix -- applies only to variables");
		}

		{
			auto value = get_number(left);
			auto expr = static_pointer_cast<var_expression>(pe->get_left());
			environment_->assign(expr->get_name(), value - 1);
			return value;
		}

		break;
	default:
		//TODO: ERROR?
		break;
	}

	// Should not reach here
	return nil_value_tag;
}


clox::interpreting::evaluating_result
clox::interpreting::interpreter::visit_literal_expression(
		const std::shared_ptr<parsing::literal_expression>& expression)
{
	return interpreter::literal_value_to_interpreting_result(expression->get_value());
}

clox::interpreting::evaluating_result
clox::interpreting::interpreter::visit_grouping_expression(
		const std::shared_ptr<parsing::grouping_expression>& expression)
{
	return evaluate(expression->get_expr());
}

std::string clox::interpreting::interpreter::result_to_string(
		const token& error_prone,
		const clox::interpreting::evaluating_result& res)
{
	return visit(evaluating_result_stringify_visitor{ error_prone }, res);
}

void
clox::interpreting::interpreter::interpret(const std::vector<std::shared_ptr<parsing::statement>>& stmts, bool repl)
{
	try
	{
		repl_ = repl;
		auto _ = gsl::finally([this]
		{
			repl_ = false;
		});

		for (const auto& s: stmts)
		{
			execute(s);
		}
	}
	catch (const clox::interpreting::runtime_error& re)
	{
		clox::logging::logger::instance().runtime_error(re);
	}
}

clox::interpreting::evaluating_result
clox::interpreting::interpreter::literal_value_to_interpreting_result(const literal_value_type& value)
{
	return std::visit([](auto&& value) -> evaluating_result
	{
		using T = std::decay_t<decltype(value)>;
		if constexpr (std::is_same_v<T, floating_literal_type> ||
					  std::is_same_v<T, integer_literal_type> ||
					  std::is_same_v<T, bool> ||
					  std::is_same_v<T, string> ||
					  std::is_same_v<T, nil_value_tag_type>)
		{
			return value;
		}
		else
		{
			throw invalid_argument("value");
		}
	}, value);
}

clox::interpreting::evaluating_result clox::interpreting::interpreter::evaluate(const shared_ptr<expression>& expr)
{
	return accept(*expr, *dynamic_cast<expression_visitor<evaluating_result>*>(this));
}


bool clox::interpreting::interpreter::is_truthy(clox::interpreting::evaluating_result e)
{
	// Ruby’s rule: false and nil are false, and everything else is truthy
	if (holds_alternative<nil_value_tag_type>(e))return false;
	if (holds_alternative<bool>(e))return get<bool>(e);
	return true;
}

bool clox::interpreting::interpreter::is_equal(const token& op, clox::interpreting::evaluating_result lhs,
		clox::interpreting::evaluating_result rhs)
{
	if (holds_alternative<nil_value_tag_type>(lhs) && holds_alternative<nil_value_tag_type>(rhs))
	{
		return true;
	}
	else if (holds_alternative<nil_value_tag_type>(lhs) || holds_alternative<nil_value_tag_type>(rhs))
	{
		return false;
	}

	if (is_number(lhs))
	{
		if (!is_number(rhs))
		{
			throw clox::interpreting::runtime_error(op, "Operands must be the same type.");
		}
		return get_number(lhs) == get_number(rhs);
	}
	else if (holds_alternative<string>(lhs))
	{
		if (!holds_alternative<string>(rhs))
		{
			throw clox::interpreting::runtime_error(op, "Operands must be the same type.");
		}
		return get<string>(lhs) == get<string>(rhs);
	}
	else if (holds_alternative<bool>(lhs))
	{
		if (!holds_alternative<bool>(rhs))
		{
			throw clox::interpreting::runtime_error(op, "Operands must be the same type.");
		}
		return get<bool>(lhs) == get<bool>(rhs);
	}

	//FIXME :error?
	return false;
}


void interpreter::visit_expression_statement(const shared_ptr<parsing::expression_statement>& es)
{
	auto res = evaluate(es->get_expr());
	if (repl_)
	{
		if (holds_alternative<nil_value_tag_type>(res))
		{
			console_->out() << "nil" << endl;
		}
		else if (is_number(res))
		{
			console_->out() << std::to_string(get_number(res)) << endl;
		}
		else if (holds_alternative<bool>(res))
		{
			console_->out() << string{ get<bool>(res) ? "true" : "false" } << endl;
		}
		else if (holds_alternative<string>(res))
		{
			console_->out() << get<string>(res) << endl;
		}
		else
		{
			console_->out() << "Unevaluable expression." << endl;
		}
	}
}

void interpreter::visit_print_statement(const shared_ptr<parsing::print_statement>& ps)
{
	auto val = evaluate(ps->get_expr());
	console_->out() << result_to_string(ps->get_keyword(), val) << endl;
}

void interpreter::execute(const shared_ptr<parsing::statement>& s)
{
	accept(*s, *dynamic_cast<statement_visitor<void>*>(this));
}

evaluating_result interpreter::visit_var_expression(const std::shared_ptr<var_expression>& e)
{
	auto opt_val = variable_lookup(e->get_name(), e);
	if (opt_val.has_value())
	{
		return *opt_val;
	}

	throw runtime_error{ e->get_name(), std::format("Undefined variable '{}'.", e->get_name().lexeme()) };
}

void interpreter::visit_variable_statement(const std::shared_ptr<variable_statement>& stmt)
{
	decltype(evaluate(stmt->get_initializer())) value{ nil_value_tag };
	if (stmt->get_initializer())
	{
		value = evaluate(stmt->get_initializer());
	}

	environment_->put(stmt->get_name().lexeme(), value);
}

evaluating_result interpreter::visit_assignment_expression(const std::shared_ptr<struct assignment_expression>& ae)
{
	auto val = evaluate(ae->get_value());
//	environment_->assign(ae->get_name(), val);
	variable_assign(ae->get_name(), ae, val);
	return val;
}

void interpreter::visit_block_statement(const std::shared_ptr<block_statement>& bs)
{
	execute_block(bs->get_stmts(), make_shared<environment>(this->environment_));
}

void
interpreter::execute_block(const vector<std::shared_ptr<parsing::statement>>& stmts, const shared_ptr<environment>& env)
{
	auto prev = this->environment_;

	auto _ = gsl::finally([&prev, this]
	{
		this->environment_ = prev;
	});

	this->environment_ = env;

	for (const auto& stmt: stmts)
	{
		execute(stmt);
	}
}

void interpreter::visit_if_statement(const std::shared_ptr<if_statement>& if_stmt)
{
	if (is_truthy(evaluate(if_stmt->get_cond())))
	{
		execute(if_stmt->get_true_stmt());
	}
	else if (if_stmt->get_false_stmt())
	{
		execute(if_stmt->get_false_stmt());
	}
}

evaluating_result interpreter::visit_ternary_expression(const std::shared_ptr<ternary_expression>& te)
{
	if (is_truthy(evaluate(te->get_cond())))
	{
		return evaluate(te->get_true_expr());
	}
	else
	{
		return evaluate(te->get_false_expr());
	}
}

evaluating_result interpreter::visit_logical_expression(const std::shared_ptr<logical_expression>& le)
{
	auto left = evaluate(le->get_left());

	// handle short circuit
	if (auto op = le->get_op();op.type() == token_type::OR)
	{
		if (is_truthy(left))
		{
			return left;
		}
	}
	else if (op.type() == token_type::AND)
	{
		if (!is_truthy(left))
		{
			return left;
		}
	}

	return evaluate(le->get_right());
}

void interpreter::visit_while_statement(const std::shared_ptr<while_statement>& ws)
{
	while (is_truthy(evaluate(ws->get_cond())))
	{
		execute(ws->get_body());
	}
}

evaluating_result interpreter::visit_call_expression(const std::shared_ptr<call_expression>& ce)
{
	auto callee = evaluate(ce->get_callee());

	vector<evaluating_result> args{};
	for (const auto& arg: ce->get_args())
	{
		args.push_back(evaluate(arg));
	}

	if (!holds_alternative<shared_ptr<callable>>(callee) && !holds_alternative<overloaded_functions>(callee))
	{
		throw clox::interpreting::runtime_error{ ce->get_paren(), "Expression isn't callable." };
	}


	shared_ptr<callable> func{ nullptr };

	if (holds_alternative<shared_ptr<callable>>(callee))
	{
		func = get<shared_ptr<callable>>(callee);
	}
	else if (holds_alternative<overloaded_functions>(callee))
	{
		auto binding = locals_->get(ce).value();
		if (auto func_binding = dynamic_pointer_cast<function_binding>(binding);func_binding)
		{
			func = get<overloaded_functions>(callee).at(func_binding->statement());
		}
		else if (!func_binding)
		{
			throw clox::interpreting::runtime_error{ ce->get_paren(),
													 "Internal: function binding required." };
		}
	}

	return func->call(this, ce, args);
}

void interpreter::install_native_functions()
{
	globals_->put("clock", make_shared<clock_func>());
}

void interpreter::visit_function_statement(const std::shared_ptr<function_statement>& stmt)
{
	auto func = make_shared<lox_function>(stmt, environment_);
	environment_->put(stmt->get_name().lexeme(), stmt, func);
}

void interpreter::visit_return_statement(const std::shared_ptr<return_statement>& rs)
{
	evaluating_result ret{ nil_value_tag };
	if (auto val = rs->get_val();val)
	{
		ret = evaluate(val);
	}

	throw return_value{ ret };
}


std::optional<evaluating_result> interpreter::variable_lookup(const token& tk, const shared_ptr<expression>& expr)
{
	auto local = locals_->get(expr);
	if (!local.has_value())
	{
		return globals_->get(tk);
	}

	auto binding = local.value();

	int64_t dist = -1;
	if (binding->type() != binding_type::BINDING_VARIABLE)
	{
		throw clox::interpreting::runtime_error{ tk,
												 std::format("{} is not a variable.",
														 tk.lexeme()) };
	}
	else
	{
		dist = static_pointer_cast<variable_binding>(binding)->depth();
	}

	if (dist != -1)
	{
		return environment_->get_at(tk.lexeme(), dist);
	}
	else
	{
		return globals_->get(tk);
	}
}

void
interpreter::variable_assign(const token& tk, const shared_ptr<parsing::expression>& expr, const evaluating_result& val)
{
	auto local = locals_->get(expr);
	if (!local.has_value())
	{
		globals_->assign(tk, val);
	}

	auto binding = local.value();

	int64_t dist = -1;
	if (binding->type() != resolving::binding_type::BINDING_VARIABLE)
	{
		throw clox::interpreting::runtime_error{ tk,
												 std::format("{} is not a variable.",
														 tk.lexeme()) };
	}
	else
	{
		dist = static_pointer_cast<variable_binding>(binding)->depth();
	}

	if (dist != -1)
	{
		environment_->assign_at(tk, val, dist);
	}
	else
	{
		globals_->assign(tk, val);
	}
}

void interpreter::visit_class_statement(const std::shared_ptr<class_statement>& cls)
{
	evaluating_result base_val{};
	shared_ptr<lox_class> base{ nullptr };

	if (cls->get_base_class())
	{
		base_val = evaluate(cls->get_base_class());

		if (!holds_alternative<shared_ptr<callable>>(base_val) ||
			!(base = dynamic_pointer_cast<lox_class>(get<shared_ptr<callable>>(base_val))))
		{
			throw clox::interpreting::runtime_error{ cls->get_base_class()->get_name(),
													 std::format("'{}' is not inheritable.",
															 cls->get_base_class()->get_name().lexeme()) };
		}
	}

	environment_->put(cls->get_name().lexeme(), nil_value_tag);

	if (cls->get_base_class())
	{
		environment_ = make_shared<environment>(environment_);
		environment_->put("base", base);
	}

	auto lox_cls = make_shared<lox_class>(cls->get_name().lexeme(), base);


	for (const auto& me: cls->get_methods())
	{
		lox_cls->put_method(me->get_name().lexeme(), me, make_shared<lox_function>(me, environment_,
				me->get_func_type() == parsing::function_statement_type::FST_CTOR));
	}


	if (base)
	{
		environment_ = environment_->parent();
	}

	environment_->assign(cls->get_name(),
			lox_cls);
}

evaluating_result interpreter::visit_get_expression(const std::shared_ptr<get_expression>& expr)
{
	auto obj = evaluate(expr->get_object());

	if (holds_alternative<shared_ptr<lox_instance>>(obj))
	{
		return get<shared_ptr<lox_instance>>(obj)->get(expr->get_name());
	}

	throw clox::interpreting::runtime_error{ expr->get_name(),
											 std::format("Properties access is not allowed except instances") };
}

evaluating_result interpreter::visit_set_expression(const std::shared_ptr<set_expression>& se)
{
	auto obj = evaluate(se->get_object());

	if (!holds_alternative<shared_ptr<lox_instance>>(obj))
	{
		throw clox::interpreting::runtime_error{ se->get_name(),
												 std::format("Only instances have fields") };
	}

	auto val = evaluate(se->get_val());
	get<shared_ptr<lox_instance>>(obj)->set(se->get_name(), val);
	return val;
}

evaluating_result interpreter::visit_this_expression(const std::shared_ptr<this_expression>& this_expr)
{
	auto this_val = variable_lookup(this_expr->get_keyword(), this_expr);
	if (this_val)
	{
		return this_val.value();
	}


	throw runtime_error{ this_expr->get_keyword(), std::format("Internal error: cannot look up 'this'.") };
}

evaluating_result interpreter::visit_base_expression(const std::shared_ptr<base_expression>& expr)
{
	// FIXME: should support fields
	auto symbol = locals_->get(expr).value(); // it must exist granted by the resolver
	auto dist = static_pointer_cast<variable_binding>(symbol)->depth();

	auto base = static_pointer_cast<lox_class>(get<shared_ptr<callable>>(environment_->get_at("base", dist).value()));

	auto this_inst = get<shared_ptr<lox_instance>>(environment_->get_at("this", dist - 1).value());

	auto method = base->lookup_method(expr->get_member().lexeme());

	if (!method)
	{
		throw clox::interpreting::runtime_error{ expr->get_member(),
												 std::format("'{}' is undefined.", expr->get_member().lexeme()) };
	}

	auto bound_methods = method.value();
	for (auto& m: bound_methods)
	{
		m.second = dynamic_pointer_cast<lox_function>(m.second)->bind(this_inst);
	}

	return bound_methods;
}

long double interpreter::get_number(const evaluating_result& num)
{
	return std::visit([](auto&& val) -> floating_literal_type
	{
		using T = std::decay_t<decltype(val)>;
		if constexpr(std::is_same_v<T, floating_literal_type> ||
					 std::is_same_v<T, integer_literal_type>)
		{
			return val;
		}

		return NAN;
	}, num);
}

bool interpreter::is_number(const evaluating_result& e)
{
	return holds_alternative<long double>(e) || holds_alternative<long long>(e);
}

evaluating_result
interpreter::visit_initializer_list_expression(const std::shared_ptr<initializer_list_expression>& ile)
{
	return clox::interpreting::evaluating_result();
}

void interpreter::visit_foreach_statement(const std::shared_ptr<foreach_statement>& fes)
{

}
