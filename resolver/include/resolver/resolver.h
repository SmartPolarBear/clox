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

#include <resolver/lox_type.h>

#include <vector>
#include <stack>
#include <memory>

namespace clox::resolving
{
class scope final
{
public:
	using name_table_type = std::unordered_map<std::string, bool>;
	using type_table_type = std::unordered_map<std::string, std::shared_ptr<lox_type>>;

	scope() = default;

	name_table_type& names()
	{
		return names_;
	}

	type_table_type& types()
	{
		return types_;
	}

private:
	mutable name_table_type names_{};
	mutable type_table_type types_{};
};

class resolver final
		: public parsing::expression_visitor<void>,
		  public parsing::statement_visitor<void>,
		  public parsing::type_expression_visitor<std::shared_ptr<lox_type>>
{
public:
	enum class [[clang::enum_extensibility(closed)]] function_type
	{
		FT_NONE,
		FT_METHOD,
		FT_CTOR,
		FT_FUNCTION,
	};

	enum class [[clang::enum_extensibility(closed)]] class_type
	{
		CT_NONE,
		CT_CLASS,
		CT_INHERITED_CLASS
	};

public:
	explicit resolver(std::shared_ptr<symbol_table> tbl);

	~resolver() = default;

	void visit_assignment_expression(const std::shared_ptr<parsing::assignment_expression>& ptr) override;

	void visit_binary_expression(const std::shared_ptr<parsing::binary_expression>& ptr) override;

	void visit_unary_expression(const std::shared_ptr<parsing::unary_expression>& ptr) override;

	void visit_literal_expression(const std::shared_ptr<parsing::literal_expression>& ptr) override;

	void visit_grouping_expression(const std::shared_ptr<parsing::grouping_expression>& ptr) override;

	void visit_this_expression(const std::shared_ptr<parsing::this_expression>& ptr) override;

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

	void visit_get_expression(const std::shared_ptr<parsing::get_expression>& ptr) override;

	void visit_set_expression(const std::shared_ptr<parsing::set_expression>& ptr) override;

	void visit_base_expression(const std::shared_ptr<parsing::base_expression>& ptr) override;

	std::shared_ptr<lox_type>
	visit_variable_type_expression(const std::shared_ptr<parsing::variable_type_expression>& ptr) override;

public:
	void resolve(const std::vector<std::shared_ptr<parsing::statement>>& stmts);

	void resolve(const std::shared_ptr<parsing::statement>& stmt);

	std::shared_ptr<lox_type> resolve(const std::shared_ptr<parsing::type_expression>& expr);

	void resolve(const std::shared_ptr<parsing::expression>& expr);

private:
	std::shared_ptr<lox_type> type_error(const scanning::token& tk);

	void resolve_local(const std::shared_ptr<parsing::expression>& expr, const scanning::token& tk);

	void resolve_function(const std::shared_ptr<parsing::function_statement>& func, function_type type);

	std::shared_ptr<lox_type> type_lookup(const scanning::token& tk);

	void check_type_assignment(const scanning::token& tk, const lox_type& left, const lox_type& right);

	void scope_begin();

	void scope_end();

	void declare_name(const scanning::token& t);

	void define_name(const scanning::token& t);

	void define_type(const scanning::token& t, const lox_type& type, uint64_t depth = 1);

	std::shared_ptr<scope> scope_top()
	{
		return scopes_.back();
	}

	std::optional<bool> scope_top_find(const std::string& key)
	{
		auto top = scope_top();
		if (!top->names().contains(key))return std::nullopt;
		else return top->names().at(key);
	}

	void scope_push(const std::shared_ptr<scope>& s)
	{
		scopes_.push_back(s);
	}

	void scope_pop()
	{
		scopes_.pop_back();
	}


	std::shared_ptr<scope> global_scope_{ nullptr };

	std::vector<std::shared_ptr<scope>> scopes_{ global_scope_ };

	function_type cur_func_{ function_type::FT_NONE };

	class_type cur_cls_{ class_type::CT_NONE };


	std::shared_ptr<symbol_table> symbols_{ nullptr };
};
}