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

#include <base/iterable_stack.h>
#include <base/generator.h>
#include <logger/logger.h>

#include <parser/gen/parser_classes.inc>

#include <interpreter/classic/interpreter.h>

#include "type/lox_type.h"
#include "type/callable_type.h"
#include "type/class_type.h"
#include "type/instance_type.h"
#include <resolver/binding.h>
#include <resolver/function.h>

#include "symbol/scope.h"
#include "symbol/scope_collection.h"


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


class resolver final
		: public parsing::expression_visitor<std::shared_ptr<lox_type>>,
		  public parsing::type_expression_visitor<std::shared_ptr<lox_type>>,
		  public parsing::statement_visitor<void>
{
public:


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


public:


	explicit resolver();

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

	std::shared_ptr<lox_type> visit_lambda_expression(const std::shared_ptr<parsing::lambda_expression>& ptr) override;

	std::shared_ptr<lox_type>
	visit_list_initializer_expression(const std::shared_ptr<parsing::list_initializer_expression>& ptr) override;

	std::shared_ptr<lox_type>
	visit_map_initializer_expression(const std::shared_ptr<parsing::map_initializer_expression>& ptr) override;

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

	void visit_foreach_statement(const std::shared_ptr<parsing::foreach_statement>& ptr) override;

	// type expressions

	std::shared_ptr<lox_type>
	visit_variable_type_expression(const std::shared_ptr<parsing::variable_type_expression>& ptr) override;

	std::shared_ptr<lox_type>
	visit_union_type_expression(const std::shared_ptr<parsing::union_type_expression>& ptr) override;

	std::shared_ptr<lox_type>
	visit_callable_type_expression(const std::shared_ptr<parsing::callable_type_expression>& ptr) override;

	std::shared_ptr<lox_type>
	visit_list_type_expression(const std::shared_ptr<parsing::list_type_expression>& ptr) override;

	std::shared_ptr<lox_type>
	visit_map_type_expression(const std::shared_ptr<parsing::map_type_expression>& ptr) override;

public:
	void resolve(const std::vector<std::shared_ptr<parsing::statement>>& stmts);

	void resolve(const std::shared_ptr<parsing::statement>& stmt);

	std::shared_ptr<lox_type> resolve(const std::shared_ptr<parsing::type_expression>& expr);

	std::shared_ptr<lox_type> resolve(const std::shared_ptr<parsing::expression>& expr);


	[[nodiscard]] std::shared_ptr<scope> global_scope() const
	{
		return global_scope_;
	}

	[[nodiscard]] std::optional<function_id_type> function_id(const std::shared_ptr<parsing::statement>& stmt) const;

private:

	std::shared_ptr<lox_type> type_error(const clox::scanning::token& tk, const std::string& msg);


	std::shared_ptr<symbol> resolve_local(const std::shared_ptr<parsing::expression>& expr, const scanning::token& tk);

	std::shared_ptr<upvalue>
	resolve_upvalue(const std::shared_ptr<function_scope>& cur, const std::shared_ptr<function_scope>& bottom,
			const std::shared_ptr<symbol>& sym);

	std::shared_ptr<lox_type> resolve_function_call(const std::shared_ptr<parsing::call_expression>& call,
			const std::shared_ptr<lox_overloaded_metatype>& callee);

	/// Resolve what can be confirmed just by the signature of the function.
	/// \param func
	/// \param type
	/// \return std::shared_ptr<lox_callable_type> resulting in the function, or std::shared_ptr<lox_type> if failed
	std::shared_ptr<lox_type> resolve_function_decl(const std::shared_ptr<parsing::function_statement>& func);

	/// Resolve the function body
	/// \param func
	/// \param type
	void resolve_function_body(const std::shared_ptr<parsing::function_statement>& func, env_function_type type);


	std::tuple<std::shared_ptr<lox_class_type>,
			std::shared_ptr<lox_class_type>,
			std::shared_ptr<lox_instance_type>>
	resolve_class_type_decl(const std::shared_ptr<parsing::class_statement>& cls);

	void complement_default_members(const std::shared_ptr<parsing::class_statement>& cls,
			const std::shared_ptr<lox_class_type>& class_type);

	void resolve_class_members(const std::shared_ptr<parsing::class_statement>& cls,
			const std::shared_ptr<lox_class_type>& class_type);

	std::shared_ptr<lox_type> type_lookup(const scanning::token& tk);

	type_compatibility check_type_assignment(const scanning::token& tk, const std::shared_ptr<lox_type>& left,
			const std::shared_ptr<lox_type>& right);

	using overloadable_operator_type_check_result = std::tuple<std::shared_ptr<lox_type>,
			bool,
			std::optional<std::tuple<std::shared_ptr<parsing::statement>, std::shared_ptr<lox_callable_type>>>>;

	overloadable_operator_type_check_result
	check_type_binary_expression(const scanning::token& tk, const std::shared_ptr<lox_type>& left,
			const std::shared_ptr<lox_type>& right);

	overloadable_operator_type_check_result
	check_type_binary_expression_primitive(const scanning::token& tk, const std::shared_ptr<lox_type>& left,
			const std::shared_ptr<lox_type>& right);

	overloadable_operator_type_check_result
	check_type_binary_expression_class(const scanning::token& tk, const std::shared_ptr<lox_type>& left,
			const std::shared_ptr<lox_type>& right);

	type_compatibility check_type_unary_expression(const scanning::token& tk, const std::shared_ptr<lox_type>& left);

	type_compatibility check_type_postfix_expression(const scanning::token& tk, const std::shared_ptr<lox_type>& l,
			const std::shared_ptr<lox_type>& r);


	type_compatibility check_type_ternary_expression(const scanning::token& tk, const std::shared_ptr<lox_type>& left,
			const std::shared_ptr<lox_type>& right);

	type_compatibility check_type_logical_expression(const scanning::token& tk, const std::shared_ptr<lox_type>& left,
			const std::shared_ptr<lox_type>& right);

	using call_resolving_check_result = std::tuple<std::shared_ptr<lox_type>, bool>;

	call_resolving_check_result check_type_call_expression(const std::shared_ptr<parsing::call_expression>& ce,
			const std::shared_ptr<lox_type>& callee,
			const std::vector<std::shared_ptr<lox_type>>& args);

	void scope_begin();

	void scope_begin(function_id_type func_id);


	void scope_begin(const std::shared_ptr<lox_class_type>& class_type, class_base_tag);

	void scope_begin(const std::shared_ptr<lox_class_type>& class_type, class_field_tag);


	void scope_end();


	void declare_name(const scanning::token& t, size_t dist = 0);

	function_id_type declare_function(const std::shared_ptr<parsing::function_statement>& fs, size_t dist = 0);

	void declare_name(const std::string& lexeme, const scanning::token& error_tk, size_t dist = 0);

	void define_name(const clox::scanning::token& tk, const std::shared_ptr<lox_type>& type, size_t dist = 0,
			bool occupy_slot = true);

	void define_name(const std::string& lexeme, const std::shared_ptr<lox_type>& type, size_t dist = 0,
			bool occupy_slot = true);

	/// define name for callable
	/// \param tk
	/// \param stmt
	/// \param type
	/// \param dist
	void define_function_name(const clox::scanning::token& tk,
			const std::shared_ptr<parsing::statement>& stmt,
			const std::shared_ptr<lox_callable_type>& type,
			size_t dist = 0);

	/// define name for callable
	/// \param lexeme
	/// \param stmt
	/// \param type
	/// \param dist
	void define_function_name(const std::string& lexeme, const clox::scanning::token& error_tk,
			const std::shared_ptr<parsing::statement>& stmt,
			const std::shared_ptr<lox_callable_type>& type,
			size_t dist = 0);

	void define_native_function(const std::string& name, const std::shared_ptr<lox_callable_type>& type);


	void define_type(const scanning::token& tk, const std::shared_ptr<lox_type>& type, uint64_t dist = 0);

	std::optional<bool> scope_top_find(const std::string& key, size_t dist = 0)
	{
		auto top = scopes_.peek(dist);
		if (!top->contains_name(key))return std::nullopt;
		else
		{
			auto symbol = top->name(key);
			return symbol != nullptr;
		}
	}

	inline function_id_type next_function_id(const scanning::token& error_token)
	{
		if (function_id_counter_ >= FUNCTION_ID_MAX)
		{
			logging::logger::instance().error(error_token, std::format("Too many functions have been declared"));
			return FUNCTION_ID_INVALID;
		}

		return function_id_counter_++;
	}

	void define_global_functions();

	std::shared_ptr<function_scope> global_scope_{ nullptr };

	base::iterable_stack<std::shared_ptr<scope>> scopes_{};
	base::iterable_stack<std::shared_ptr<function_scope>> function_scopes_{};

	std::unordered_map<function_id_type, std::shared_ptr<function_scope>> function_scope_ids_{};

	static inline constexpr size_t VIRTUAL_UNUSED_SLOT = SIZE_MAX;
	size_t slots_in_use_{ 1 }; // first slot is always in use

	std::stack<env_function_type> cur_func_{};
	std::stack<env_class_type> cur_class_{};

	std::stack<std::shared_ptr<lox_callable_type>> cur_func_type_{};
	std::stack<function_id_type> cur_func_id_{};
	std::stack<std::shared_ptr<lox_class_type>> cur_class_type_{};

	std::unordered_map<std::shared_ptr<parsing::statement>, function_id_type> function_ids_;

	function_id_type function_id_counter_{ FUNCTION_ID_BEGIN };
};
}