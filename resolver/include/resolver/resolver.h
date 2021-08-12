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
#include <resolver/callable_type.h>
#include <resolver/class_type.h>
#include <resolver/instance_type.h>

#include <vector>
#include <stack>
#include <map>
#include <unordered_map>
#include <memory>
#include <tuple>
#include <thread>

namespace clox::resolving
{

// tuple{result type for assignment,compatible,narrowing}
using type_compatibility = std::tuple<std::shared_ptr<lox_type>, bool, bool>;

enum class [[clang::enum_extensibility(closed)]] env_function_type
{
	FT_NONE,
	FT_METHOD,
	FT_CTOR,
	FT_FUNCTION,
};

enum class [[clang::enum_extensibility(closed)]] env_class_type
{
	CT_NONE,
	CT_CLASS,
	CT_INHERITED_CLASS
};


class scope final
{
public:
	using name_table_type = std::unordered_map<std::string, bool>;
	using type_table_type = std::unordered_map<std::string, std::shared_ptr<lox_type>>;

	friend class resolver;

	scope() = default;

	[[nodiscard]] name_table_type& names()
	{
		return names_;
	}

	[[nodiscard]]type_table_type& type_of_names()
	{
		return type_of_names_;
	}

	[[nodiscard]]type_table_type& types()
	{
		return types_;
	}

private:
	mutable name_table_type names_{};
	mutable type_table_type type_of_names_{};

	mutable type_table_type types_{};
};

class resolver final
		: public parsing::expression_visitor<std::shared_ptr<lox_type>>,
		  public parsing::type_expression_visitor<std::shared_ptr<lox_type>>,
		  public parsing::statement_visitor<void>
{

public:
	explicit resolver(std::shared_ptr<symbol_table> tbl);

	~resolver() = default;

	// expression

	std::shared_ptr<lox_type>
	visit_assignment_expression(const std::shared_ptr<parsing::assignment_expression>& ptr) override;

	std::shared_ptr<lox_type> visit_binary_expression(const std::shared_ptr<parsing::binary_expression>& ptr) override;

	std::shared_ptr<lox_type> visit_unary_expression(const std::shared_ptr<parsing::unary_expression>& ptr) override;

	std::shared_ptr<lox_type>
	visit_literal_expression(const std::shared_ptr<parsing::literal_expression>& ptr) override;

	std::shared_ptr<lox_type>
	visit_grouping_expression(const std::shared_ptr<parsing::grouping_expression>& ptr) override;

	std::shared_ptr<lox_type> visit_this_expression(const std::shared_ptr<parsing::this_expression>& ptr) override;

	std::shared_ptr<lox_type> visit_var_expression(const std::shared_ptr<parsing::var_expression>& ptr) override;

	std::shared_ptr<lox_type>
	visit_ternary_expression(const std::shared_ptr<parsing::ternary_expression>& ptr) override;

	std::shared_ptr<lox_type>
	visit_logical_expression(const std::shared_ptr<parsing::logical_expression>& ptr) override;

	std::shared_ptr<lox_type> visit_call_expression(const std::shared_ptr<parsing::call_expression>& ptr) override;

	std::shared_ptr<lox_type> visit_get_expression(const std::shared_ptr<parsing::get_expression>& ptr) override;

	std::shared_ptr<lox_type> visit_set_expression(const std::shared_ptr<parsing::set_expression>& ptr) override;

	std::shared_ptr<lox_type> visit_base_expression(const std::shared_ptr<parsing::base_expression>& ptr) override;

	std::shared_ptr<lox_type>
	visit_postfix_expression(const std::shared_ptr<parsing::postfix_expression>& ptr) override;

	// statements
	void visit_expression_statement(const std::shared_ptr<parsing::expression_statement>& ptr) override;

	void visit_print_statement(const std::shared_ptr<parsing::print_statement>& ptr) override;

	void visit_variable_statement(const std::shared_ptr<parsing::variable_statement>& ptr) override;

	void visit_block_statement(const std::shared_ptr<parsing::block_statement>& ptr) override;

	void visit_while_statement(const std::shared_ptr<parsing::while_statement>& ptr) override;

	void visit_if_statement(const std::shared_ptr<parsing::if_statement>& ptr) override;

	void visit_function_statement(const std::shared_ptr<parsing::function_statement>& ptr) override;

	void visit_return_statement(const std::shared_ptr<parsing::return_statement>& ptr) override;

	void visit_class_statement(const std::shared_ptr<parsing::class_statement>& ptr) override;

	// type expressions

	std::shared_ptr<lox_type>
	visit_variable_type_expression(const std::shared_ptr<parsing::variable_type_expression>& ptr) override;

public:
	void resolve(const std::vector<std::shared_ptr<parsing::statement>>& stmts);

	void resolve(const std::shared_ptr<parsing::statement>& stmt);

	std::shared_ptr<lox_type> resolve(const std::shared_ptr<parsing::type_expression>& expr);

	std::shared_ptr<lox_type> resolve(const std::shared_ptr<parsing::expression>& expr);

private:

	std::shared_ptr<lox_type> type_error(const clox::scanning::token& tk, const std::string& msg);

	int64_t resolve_local(const std::shared_ptr<parsing::expression>& expr, const scanning::token& tk);

	/// Resolve what can be confirmed just by the signature of the function.
	/// \param func
	/// \param type
	/// \return std::shared_ptr<lox_callable_type> resulting in the function
	std::shared_ptr<lox_callable_type> resolve_function_decl(const std::shared_ptr<parsing::function_statement>& func);

	/// Resolve the function body
	/// \param func
	/// \param type
	void resolve_function_body(const std::shared_ptr<parsing::function_statement>& func, env_function_type type);


	std::tuple<std::shared_ptr<lox_instance_type>, std::shared_ptr<lox_instance_type>>
	resolve_class_type_decl(const std::shared_ptr<parsing::class_statement>& cls);

	void resolve_class_members(const std::shared_ptr<parsing::class_statement>& cls);

	std::shared_ptr<lox_type> type_lookup(const scanning::token& tk);

	type_compatibility check_type_assignment(const scanning::token& tk, const std::shared_ptr<lox_type>& left,
			const std::shared_ptr<lox_type>& right);

	type_compatibility check_type_binary_expression(const scanning::token& tk, const std::shared_ptr<lox_type>& left,
			const std::shared_ptr<lox_type>& right);

	type_compatibility check_type_unary_expression(const scanning::token& tk, const std::shared_ptr<lox_type>& left);

	type_compatibility check_type_postfix_expression(const scanning::token& tk, const std::shared_ptr<lox_type>& right);


	type_compatibility check_type_ternary_expression(const scanning::token& tk, const std::shared_ptr<lox_type>& left,
			const std::shared_ptr<lox_type>& right);

	type_compatibility check_type_logical_expression(const scanning::token& tk, const std::shared_ptr<lox_type>& left,
			const std::shared_ptr<lox_type>& right);

	void scope_begin();

	void scope_end();

	void declare_name(const scanning::token& t, size_t dist = 0);

	void define_name(const clox::scanning::token& tk, const std::shared_ptr<lox_type>& type, size_t dist = 0);

	void define_name(const std::string& lexeme, const std::shared_ptr<lox_type>& type, size_t dist = 0);

	void define_type(const scanning::token& tk, const std::shared_ptr<lox_type>& type, uint64_t dist = 0);

	std::shared_ptr<scope> scope_top(size_t dist = 0)
	{
		return *(scopes_.rbegin() + dist);
	}


	std::optional<bool> scope_top_find(const std::string& key, size_t dist = 0)
	{
		auto top = scope_top(dist);
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

	std::stack<env_function_type> cur_func_{};
	std::stack<env_class_type> cur_class_{};

	std::stack<std::shared_ptr<lox_callable_type>> cur_func_type_{};
	std::stack<std::shared_ptr<lox_class_type>> cur_class_type_{};

	std::shared_ptr<symbol_table> symbols_{ nullptr };
};
}