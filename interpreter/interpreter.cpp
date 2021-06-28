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

#include <logger/logger.h>

#include <iostream>
#include <format>
#include <utility>

#include <gsl/gsl>

using namespace std;
using namespace clox::scanning;
using namespace clox::parsing;
using namespace clox::interpreting;


clox::interpreting::evaluating_result
clox::interpreting::interpreter::visit_binary_expression(const std::shared_ptr<binary_expression>& be)
{
	auto left = evaluate(be->get_left());
	auto right = evaluate(be->get_right());

	switch (be->get_op().type())
	{
	case scanning::token_type::GREATER:
		check_numeric_operands(be->get_op(), left, right);
		return get<long double>(left) > get<long double>(right);
	case scanning::token_type::GREATER_EQUAL:
		check_numeric_operands(be->get_op(), left, right);
		return get<long double>(left) >= get<long double>(right);
	case scanning::token_type::LESS:
		check_numeric_operands(be->get_op(), left, right);
		return get<long double>(left) < get<long double>(right);
	case scanning::token_type::LESS_EQUAL:
		check_numeric_operands(be->get_op(), left, right);
		return get<long double>(left) <= get<long double>(right);

	case scanning::token_type::MINUS:
		check_numeric_operands(be->get_op(), left, right);
		return get<long double>(left) - get<long double>(right);
	case scanning::token_type::PLUS:
		if (holds_alternative<long double>(left) && holds_alternative<long double>(right))
		{
			return get<long double>(left) + get<long double>(right);
		}
		else if (holds_alternative<string>(left) && holds_alternative<string>(right))
		{
			return get<string>(left) + get<string>(right);
		}

		throw clox::interpreting::runtime_error(be->get_op(), "Operands should be strings or numbers.");
	case scanning::token_type::SLASH:
		check_numeric_operands(be->get_op(), left, right);
		return get<long double>(left) / get<long double>(right);
	case scanning::token_type::STAR:
		check_numeric_operands(be->get_op(), left, right);
		return get<long double>(left) * get<long double>(right);


	case scanning::token_type::BANG_EQUAL:
		return !is_equal(be->get_op(), left, right);
	case scanning::token_type::EQUAL_EQUAL:
		return is_equal(be->get_op(), left, right);

	default:
		//TODO: ERROR?
		break;
	}

	// Should not reach here
	return nil_value_tag;
}

clox::interpreting::evaluating_result
clox::interpreting::interpreter::visit_unary_expression(const std::shared_ptr<unary_expression>& ue)
{
	auto right = evaluate(ue->get_right());

	switch (ue->get_op().type())
	{
	case scanning::token_type::MINUS:
		return -get<long double>(right);
	case scanning::token_type::BANG:
		return !is_truthy(right);
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

std::string clox::interpreting::interpreter::result_to_string(const clox::interpreting::evaluating_result& res)
{
	if (holds_alternative<nil_value_tag_type>(res))return "nil";

	if (holds_alternative<long double>(res))
	{
		return std::to_string(get<long double>(res));
	}
	else if (holds_alternative<bool>(res))
	{
		return string{ bool_to_string(get<bool>(res)) };
	}
	else if (holds_alternative<string>(res))
	{
		return get<string>(res);
	}

	// FIXME: error?
	return "";
}

void clox::interpreting::interpreter::interpret(vector<shared_ptr<statement>>&& t_stmts)
{
	auto stmts = t_stmts;
	try
	{
		for (const auto& s:stmts)
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
	if (holds_alternative<long double>(value))
	{
		return get<long double>(value);
	}
	else if (holds_alternative<bool>(value))
	{
		return get<bool>(value);
	}
	else if (holds_alternative<string>(value))
	{
		return get<std::string>(value);
	}
	else if (holds_alternative<nil_value_tag_type>(value))
	{
		return get<scanning::nil_value_tag_type>(value);
	}
	else
	{
		throw invalid_argument("value");
	}
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

	if (holds_alternative<long double>(lhs))
	{
		if (!holds_alternative<long double>(rhs))
		{
			throw clox::interpreting::runtime_error(op, "Operands must be the same type.");
		}
		return get<long double>(lhs) == get<long double>(rhs);
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

void clox::interpreting::interpreter::check_numeric_operands(token op, const clox::interpreting::evaluating_result& l,
		const clox::interpreting::evaluating_result& r)
{
	if (holds_alternative<long double>(l) && holds_alternative<long double>(r))return;

	throw clox::interpreting::runtime_error(std::move(op), "Operands must be numbers.");
}

constexpr std::string_view interpreter::bool_to_string(bool b)
{
	return b ? "true" : "false";
}

void interpreter::visit_expression_statement(const shared_ptr<parsing::expression_statement>& es)
{
	evaluate(es->get_expr());
}

void interpreter::visit_print_statement(const shared_ptr<parsing::print_statement>& ps)
{
	auto val = evaluate(ps->get_expr());
	cout << result_to_string(val) << endl;
}

void interpreter::execute(const shared_ptr<parsing::statement>& s)
{
	accept(*s, *dynamic_cast<statement_visitor<void>*>(this));
}

evaluating_result interpreter::visit_var_expression(const std::shared_ptr<var_expression>& e)
{
	auto opt_val = environment_->get(e->get_name());
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
	environment_->assign(ae->get_name(), val);
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

	for (const auto& stmt:stmts)
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


