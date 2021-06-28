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

#pragma once

#include <base.h>

#include <parser/gen/parser_classes.inc>

#include <interpreter/evaluating_result.h>
#include <interpreter/environment.h>

#include <scanner/nil_value.h>
#include <scanner/scanner.h>

#include <parser/parser.h>

#include <variant>
#include <string>

namespace clox::interpreting
{
class interpreter final :
		virtual parsing::expression_visitor<evaluating_result>,
		virtual parsing::statement_visitor<void>,
		public base::singleton<interpreter>
{
public:
	interpreter() : expression_visitor<evaluating_result>(),
					parsing::statement_visitor<void>(),
					base::singleton<interpreter>(),
					environment_(std::make_shared<environment>())
	{
	}

	void visit_expression_statement(const std::shared_ptr<parsing::expression_statement>& ptr) override;

	void visit_print_statement(const std::shared_ptr<parsing::print_statement>& ptr) override;

	void visit_variable_statement(const std::shared_ptr<parsing::variable_statement>& ptr) override;

	void interpret(std::vector<std::shared_ptr<parsing::statement>>&& stmts);

	evaluating_result visit_assignment_expression(const std::shared_ptr<parsing::assignment_expression>& ptr) override;

	evaluating_result visit_binary_expression(const std::shared_ptr<parsing::binary_expression>& expression) override;

	evaluating_result visit_unary_expression(const std::shared_ptr<parsing::unary_expression>& expression) override;

	evaluating_result
	visit_literal_expression(const std::shared_ptr<parsing::literal_expression>& expression) override;

	evaluating_result
	visit_grouping_expression(const std::shared_ptr<parsing::grouping_expression>& expression) override;

	evaluating_result visit_ternary_expression(const std::shared_ptr<parsing::ternary_expression>& ptr) override;

	evaluating_result visit_logical_expression(const std::shared_ptr<parsing::logical_expression>& ptr) override;

	evaluating_result
	visit_var_expression(const std::shared_ptr<parsing::var_expression>& ptr) override;

	void visit_block_statement(const std::shared_ptr<parsing::block_statement>& ptr) override;

	void visit_if_statement(const std::shared_ptr<parsing::if_statement>& ptr) override;

private:
	void execute(const std::shared_ptr<parsing::statement>& s);

	void execute_block(const std::vector<std::shared_ptr<parsing::statement>>& stmts,
			const std::shared_ptr<environment>& env);

	evaluating_result evaluate(const std::shared_ptr<parsing::expression>& expr);

	static std::string result_to_string(const evaluating_result& res);

	static constexpr std::string_view bool_to_string(bool b);

	static void check_numeric_operands(scanning::token, const evaluating_result& l, const evaluating_result& r);

	static evaluating_result literal_value_to_interpreting_result(const scanning::literal_value_type& any);

	static bool is_equal(const scanning::token& op, clox::interpreting::evaluating_result lhs,
			clox::interpreting::evaluating_result rhs);

	static bool is_truthy(evaluating_result res);

	std::shared_ptr<environment> environment_{ nullptr };
};
}
