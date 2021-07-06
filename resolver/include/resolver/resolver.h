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
// Created by cleve on 7/1/2021.
//

#pragma once

#include <parser/gen/parser_classes.inc>

#include <interpreter/interpreter.h>

#include <vector>
#include <stack>
#include <memory>

namespace clox::resolver
{
class resolver final
		: public parsing::expression_visitor<void>,
		  public parsing::statement_visitor<void>
{
public:
	enum class [[clang::enum_extensibility(closed)]] function_type
	{
		FT_NONE,
		FT_FUNCTION,
	};

public:
	explicit resolver(interpreting::interpreter* intp) : intp_(intp)
	{
	}

	~resolver() = default;

	void visit_assignment_expression(const std::shared_ptr<parsing::assignment_expression>& ptr) override;

	void visit_binary_expression(const std::shared_ptr<parsing::binary_expression>& ptr) override;

	void visit_unary_expression(const std::shared_ptr<parsing::unary_expression>& ptr) override;

	void visit_literal_expression(const std::shared_ptr<parsing::literal_expression>& ptr) override;

	void visit_grouping_expression(const std::shared_ptr<parsing::grouping_expression>& ptr) override;

	void visit_var_expression(const std::shared_ptr<parsing::var_expression>& ptr) override;

	void visit_ternary_expression(const std::shared_ptr<parsing::ternary_expression>& ptr) override;

	void visit_logical_expression(const std::shared_ptr<parsing::logical_expression>& ptr) override;

	void visit_call_expression(const std::shared_ptr<parsing::call_expression>& ptr) override;

	void visit_expression_statement(const std::shared_ptr<parsing::expression_statement>& ptr) override;

	void visit_postfix_expression(const std::shared_ptr<parsing::postfix_expression>& ptr) override;

	void visit_print_statement(const std::shared_ptr<parsing::print_statement>& ptr) override;

	void visit_variable_statement(const std::shared_ptr<parsing::variable_statement>& ptr) override;

	void visit_block_statement(const std::shared_ptr<parsing::block_statement>& ptr) override;

	void visit_while_statement(const std::shared_ptr<parsing::while_statement>& ptr) override;

	void visit_if_statement(const std::shared_ptr<parsing::if_statement>& ptr) override;

	void visit_function_statement(const std::shared_ptr<parsing::function_statement>& ptr) override;

	void visit_return_statement(const std::shared_ptr<parsing::return_statement>& ptr) override;

	void visit_class_statement(const std::shared_ptr<parsing::class_statement>& ptr) override;


public:
	void resolve(const std::vector<std::shared_ptr<parsing::statement>>& stmts);

	void resolve(const std::shared_ptr<parsing::statement>& stmt);

	void resolve(const std::shared_ptr<parsing::expression>& expr);


private:
	void resolve_local(const std::shared_ptr<parsing::expression>& expr, const scanning::token& tk);

	void resolve_function(const std::shared_ptr<parsing::function_statement>& func, function_type type);

	void scope_begin();

	void scope_end();

	void declare(const scanning::token& t);

	void define(const scanning::token& t);

	std::shared_ptr<std::unordered_map<std::string, bool>> scope_top()
	{
		return scopes_.back();
	}

	std::optional<bool> scope_top_find(const std::string& key)
	{
		auto top = scope_top();
		if (!top->contains(key))return std::nullopt;
		else return top->at(key);
	}

	void scope_push(const std::shared_ptr<std::unordered_map<std::string, bool>>& s)
	{
		scopes_.push_back(s);
	}

	void scope_pop()
	{
		scopes_.pop_back();
	}


	std::vector<std::shared_ptr<std::unordered_map<std::string, bool>>> scopes_{};

	function_type cur_func_{ function_type::FT_NONE };

	interpreting::interpreter* intp_{ nullptr };
};
}