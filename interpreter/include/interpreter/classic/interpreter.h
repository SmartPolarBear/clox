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

#include <base/base.h>

#include <helper/console.h>

#include <parser/gen/parser_classes.inc>

#include <interpreter/classic/evaluating_result.h>
#include <interpreter/classic/environment.h>
#include <resolver/binding.h>

#include <scanner/nil_value.h>
#include <scanner/scanner.h>

#include <parser/parser.h>

#include <resolver/symbol.h>


#include <variant>
#include <string>
#include "environment.h"

namespace clox::interpreting::classic
{


class interpreter final :
		virtual parsing::expression_visitor<evaluating_result>,
		virtual parsing::statement_visitor<void>
{

public:
	friend class lox_class;

	friend class lox_function;

public:
	[[nodiscard]] explicit interpreter(helper::console& cons);

public:

	void visit_expression_statement(const std::shared_ptr<parsing::expression_statement>& ptr) override;

	void visit_print_statement(const std::shared_ptr<parsing::print_statement>& ptr) override;

	void visit_variable_statement(const std::shared_ptr<parsing::variable_statement>& ptr) override;

	void interpret(const std::vector<std::shared_ptr<parsing::statement>>& stmts, bool repl = false);

	evaluating_result visit_assignment_expression(const std::shared_ptr<parsing::assignment_expression>& ptr) override;

	evaluating_result visit_binary_expression(const std::shared_ptr<parsing::binary_expression>& expression) override;

	evaluating_result visit_unary_expression(const std::shared_ptr<parsing::unary_expression>& expression) override;

	evaluating_result visit_this_expression(const std::shared_ptr<parsing::this_expression>& ptr) override;

	evaluating_result
	visit_literal_expression(const std::shared_ptr<parsing::literal_expression>& expression) override;

	evaluating_result
	visit_grouping_expression(const std::shared_ptr<parsing::grouping_expression>& expression) override;

	evaluating_result visit_ternary_expression(const std::shared_ptr<parsing::ternary_expression>& ptr) override;

	evaluating_result visit_logical_expression(const std::shared_ptr<parsing::logical_expression>& ptr) override;

	evaluating_result visit_call_expression(const std::shared_ptr<parsing::call_expression>& ptr) override;

	evaluating_result visit_postfix_expression(const std::shared_ptr<parsing::postfix_expression>& ptr) override;

	evaluating_result
	visit_list_initializer_expression(const std::shared_ptr<parsing::list_initializer_expression>& ptr) override;

	evaluating_result
	visit_map_initializer_expression(const std::shared_ptr<parsing::map_initializer_expression>& ptr) override;

	evaluating_result
	visit_var_expression(const std::shared_ptr<parsing::var_expression>& ptr) override;

	evaluating_result visit_get_expression(const std::shared_ptr<parsing::get_expression>& ptr) override;

	void visit_block_statement(const std::shared_ptr<parsing::block_statement>& ptr) override;

	void visit_if_statement(const std::shared_ptr<parsing::if_statement>& ptr) override;

	void visit_while_statement(const std::shared_ptr<parsing::while_statement>& ws) override;

	void visit_function_statement(const std::shared_ptr<parsing::function_statement>& ptr) override;

	void visit_return_statement(const std::shared_ptr<parsing::return_statement>& ptr) override;

	void visit_class_statement(const std::shared_ptr<parsing::class_statement>& ptr) override;

	evaluating_result visit_set_expression(const std::shared_ptr<parsing::set_expression>& ptr) override;

	evaluating_result visit_base_expression(const std::shared_ptr<parsing::base_expression>& ptr) override;

	evaluating_result visit_lambda_expression(const std::shared_ptr<parsing::lambda_expression>& ptr) override;

	void visit_foreach_statement(const std::shared_ptr<parsing::foreach_statement>& ptr) override;

public:
	void execute(const std::shared_ptr<parsing::statement>& s);

	void execute_block(const std::vector<std::shared_ptr<parsing::statement>>& stmts,
			const std::shared_ptr<classic::environment>& env);

private:

	void install_native_functions();

	evaluating_result evaluate(const std::shared_ptr<parsing::expression>& expr);

	std::optional<evaluating_result>
	variable_lookup(const scanning::token& tk, const std::shared_ptr<parsing::expression>& expr);

	void variable_assign(const scanning::token& tk, const std::shared_ptr<parsing::expression>& expr,
			const evaluating_result& val);

	static std::string result_to_string(const scanning::token& error_prone, const evaluating_result& res);

	static long double get_number(const evaluating_result& l);

	static bool is_number(const evaluating_result& e);

	static evaluating_result literal_value_to_interpreting_result(const scanning::literal_value_type& any);

	static bool is_equal(const scanning::token& op, evaluating_result lhs, evaluating_result rhs);

	static bool is_truthy(evaluating_result res);


	mutable bool repl_{ false };

	mutable helper::console* console_{ nullptr };

	std::shared_ptr<classic::environment> globals_{ nullptr };
	std::shared_ptr<classic::environment> environment_{ nullptr };

};
}
