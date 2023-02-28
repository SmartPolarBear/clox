// Copyright (c) 2022 SmartPolarBear
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
// Created by cleve on 4/29/2022.
//

#pragma once

#include <parser/gen/parser_classes.inc>

namespace clox::resolving::scoping
{
class scoper final
	: public parsing::expression_visitor<bool>,
	  public parsing::type_expression_visitor<void>,
	  public parsing::statement_visitor<void>
{
 public:
	bool visit_assignment_expression(const std::shared_ptr<parsing::assignment_expression>& ptr) override;
	bool visit_binary_expression(const std::shared_ptr<parsing::binary_expression>& ptr) override;
	bool visit_unary_expression(const std::shared_ptr<parsing::unary_expression>& ptr) override;
	bool visit_this_expression(const std::shared_ptr<parsing::this_expression>& ptr) override;
	bool visit_base_expression(const std::shared_ptr<parsing::base_expression>& ptr) override;
	bool visit_postfix_expression(const std::shared_ptr<parsing::postfix_expression>& ptr) override;
	bool visit_literal_expression(const std::shared_ptr<parsing::literal_expression>& ptr) override;
	bool visit_grouping_expression(const std::shared_ptr<parsing::grouping_expression>& ptr) override;
	bool visit_lambda_expression(const std::shared_ptr<parsing::lambda_expression>& ptr) override;
	bool visit_var_expression(const std::shared_ptr<parsing::var_expression>& ptr) override;
	bool visit_ternary_expression(const std::shared_ptr<parsing::ternary_expression>& ptr) override;
	bool visit_logical_expression(const std::shared_ptr<parsing::logical_expression>& ptr) override;
	bool visit_call_expression(const std::shared_ptr<parsing::call_expression>& ptr) override;
	bool visit_list_initializer_expression(const std::shared_ptr<parsing::list_initializer_expression>& ptr) override;
	bool visit_map_initializer_expression(const std::shared_ptr<parsing::map_initializer_expression>& ptr) override;
	bool visit_get_expression(const std::shared_ptr<parsing::get_expression>& ptr) override;
	bool visit_set_expression(const std::shared_ptr<parsing::set_expression>& ptr) override;
	void visit_expression_statement(const std::shared_ptr<parsing::expression_statement>& ptr) override;
	void visit_print_statement(const std::shared_ptr<parsing::print_statement>& ptr) override;
	void visit_variable_statement(const std::shared_ptr<parsing::variable_statement>& ptr) override;
	void visit_block_statement(const std::shared_ptr<parsing::block_statement>& ptr) override;
	void visit_while_statement(const std::shared_ptr<parsing::while_statement>& ptr) override;
	void visit_foreach_statement(const std::shared_ptr<parsing::foreach_statement>& ptr) override;
	void visit_if_statement(const std::shared_ptr<parsing::if_statement>& ptr) override;
	void visit_function_statement(const std::shared_ptr<parsing::function_statement>& ptr) override;
	void visit_return_statement(const std::shared_ptr<parsing::return_statement>& ptr) override;
	void visit_class_statement(const std::shared_ptr<parsing::class_statement>& ptr) override;
	void visit_variable_type_expression(const std::shared_ptr<parsing::variable_type_expression>& ptr) override;
	void visit_union_type_expression(const std::shared_ptr<parsing::union_type_expression>& ptr) override;
	void visit_list_type_expression(const std::shared_ptr<parsing::list_type_expression>& ptr) override;
	void visit_map_type_expression(const std::shared_ptr<parsing::map_type_expression>& ptr) override;
	void visit_callable_type_expression(const std::shared_ptr<parsing::callable_type_expression>& ptr) override;

 private:
	void resolve(const std::vector<std::shared_ptr<parsing::statement>>& stmts);

	void resolve(const std::shared_ptr<parsing::statement>& stmt);

	void resolve(const std::shared_ptr<parsing::type_expression>& expr);

	bool resolve(const std::shared_ptr<parsing::expression>& expr);
};
}