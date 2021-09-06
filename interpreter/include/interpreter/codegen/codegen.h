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
// Created by cleve on 9/4/2021.
//

#pragma once

#include <parser/gen/parser_classes.inc>
#include <parser/gen/parser_base.inc>

#include <interpreter/vm/chunk.h>

#include <interpreter/codegen/exceptions.h>

#include <concepts>

namespace clox::interpreting::compiling
{
class codegen final
		: virtual parsing::expression_visitor<void>,
		  virtual parsing::statement_visitor<void>
{
public:
	codegen() = default;


	void visit_assignment_expression(const std::shared_ptr<parsing::assignment_expression>& ptr) override;

	void visit_binary_expression(const std::shared_ptr<parsing::binary_expression>& ptr) override;

	void visit_unary_expression(const std::shared_ptr<parsing::unary_expression>& ptr) override;

	void visit_this_expression(const std::shared_ptr<parsing::this_expression>& ptr) override;

	void visit_base_expression(const std::shared_ptr<parsing::base_expression>& ptr) override;

	void visit_initializer_list_expression(const std::shared_ptr<parsing::initializer_list_expression>& ptr) override;

	void visit_postfix_expression(const std::shared_ptr<parsing::postfix_expression>& ptr) override;

	void visit_literal_expression(const std::shared_ptr<parsing::literal_expression>& ptr) override;

	void visit_grouping_expression(const std::shared_ptr<parsing::grouping_expression>& ptr) override;

	void visit_var_expression(const std::shared_ptr<parsing::var_expression>& ptr) override;

	void visit_ternary_expression(const std::shared_ptr<parsing::ternary_expression>& ptr) override;

	void visit_logical_expression(const std::shared_ptr<parsing::logical_expression>& ptr) override;

	void visit_call_expression(const std::shared_ptr<parsing::call_expression>& ptr) override;

	void visit_get_expression(const std::shared_ptr<parsing::get_expression>& ptr) override;

	void visit_set_expression(const std::shared_ptr<parsing::set_expression>& ptr) override;

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

public:
	void generate(const std::vector<std::shared_ptr<parsing::statement>>& stmts);


private:

	void generate(const std::shared_ptr<parsing::statement>& s);

	void generate(const std::shared_ptr<parsing::expression>& s);

	std::shared_ptr<vm::chunk> current();

	void emit_byte(uint16_t byte);

	template<std::convertible_to<uint16_t> ...Args>
	void emit_bytes(const Args& ...arg)
	{
		(emit_byte((uint16_t)arg), ...);
	}

	void emit_return();

	void emit_constant(const vm::value &val);

	uint16_t make_constant(const vm::value &val);

	std::shared_ptr<vm::chunk> current_chunk_{};
};
}