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
#include <interpreter/vm/heap.h>
#include <interpreter/vm/opcode.h>
#include <interpreter/vm/closure_object.h>

#include <interpreter/codegen/exceptions.h>

#include <resolver/binding.h>

#include <concepts>
#include <string>

namespace clox::interpreting::compiling
{
class codegen final
		: virtual parsing::expression_visitor<void>,
		  virtual parsing::statement_visitor<void>
{
public:
	class local_scope
	{
	public:
		enum class scope_type
		{
			TOP, FUNCTION
		};

		using slot_type = int64_t;
		static inline constexpr slot_type GLOBAL_SLOT = -1;
	public:
		explicit local_scope(scope_type t)
				: type_(t)
		{
			locals_.insert_or_assign("", 0); // VM's slot 0 is for internal usage.
		}

		~local_scope() = default;


		scope_type type() const
		{
			return type_;
		}

		[[nodiscard]] size_t count() const
		{
			return count_;
		}

		void declare(const std::string& name, slot_type slot)
		{
			locals_.insert_or_assign(name, slot);
			count_++;
		}

		[[nodiscard]] std::optional<slot_type> find(const std::string& name)
		{
			if (!locals_.contains(name))
			{
				return std::nullopt;
			}

			return locals_.at(name);
		}

		void add_function(const std::shared_ptr<parsing::function_statement>& stmt, vm::chunk::code_type id,
				vm::chunk::code_type constant)
		{
			function_map_[stmt] = std::make_pair(id, constant);
		}

		bool contains_function(const std::shared_ptr<parsing::statement>& stmt)
		{
			return function_map_.contains(stmt);
		}

		std::tuple<vm::chunk::code_type, vm::chunk::code_type> find(const std::shared_ptr<parsing::statement>& stmt)
		{
			return function_map_.at(stmt);
		}

	private:
		std::unordered_map<std::shared_ptr<parsing::statement>, std::tuple<vm::chunk::code_type, vm::chunk::code_type> > function_map_{};

		std::unordered_map<std::string, slot_type> locals_{};
		size_t count_{ 0 };

		scope_type type_{};
	};

	static inline constexpr auto PATCHABLE_PLACEHOLDER = std::numeric_limits<vm::full_opcode_type>::max();
public:
	explicit codegen(std::shared_ptr<vm::object_heap> heap, std::shared_ptr<resolving::binding_table> table);


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

	void visit_lambda_expression(const std::shared_ptr<parsing::lambda_expression>& ptr) override;

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

	vm::closure_object_raw_pointer top_level();

private:

	std::shared_ptr<vm::chunk> current();

	void generate(const std::shared_ptr<parsing::statement>& s);

	void generate(const std::shared_ptr<parsing::expression>& s);

	void scope_begin();

	void scope_begin(local_scope::scope_type type);

	void scope_end();

	void define_global_variable([[maybe_unused]]const std::string& name, vm::chunk::code_type global);

	void declare_local_variable(const std::string& name, size_t depth = 0);

	void function_push(vm::function_object_raw_pointer func);

	vm::function_object_raw_pointer function_pop();

	vm::function_object_raw_pointer function_top();

	///
	/// \param name
	/// \return {optional<slot>, is global}
	std::tuple<std::optional<vm::chunk::code_type>, bool> variable_lookup(const std::string& name);

	static inline bool is_variable_lookup_failure(const std::tuple<std::optional<vm::chunk::code_type>, bool>& rets)
	{
		const auto&[slot, global]=rets;
		return !slot && !global;
	}

	static inline vm::chunk::code_type variable_slot(const std::tuple<std::optional<vm::chunk::code_type>, bool>& rets)
	{
		const auto&[slot, global]=rets;
		return slot.value();
	}

	static inline vm::chunk::code_type
	is_global_variable(const std::tuple<std::optional<vm::chunk::code_type>, bool>& rets)
	{
		const auto&[slot, global]=rets;
		return global;
	}

	std::optional<std::tuple<vm::chunk::code_type, vm::chunk::code_type>>
	function_lookup(const std::shared_ptr<parsing::statement>& stmt);

	bool is_function_lookup_failure(const std::optional<std::tuple<vm::chunk::code_type, vm::chunk::code_type>>& rets)
	{
		return !rets.has_value();
	}

	void emit_code(vm::full_opcode_type byte);

	template<std::convertible_to<vm::full_opcode_type> ...Args>
	void emit_codes(const Args& ...arg)
	{
		(emit_code((vm::full_opcode_type)arg), ...);
	}

	void emit_return();

	vm::chunk::code_type emit_constant(const vm::value& val);

	vm::chunk::difference_type emit_jump(vm::full_opcode_type jmp);

	void patch_jump(vm::chunk::difference_type pos);

	void emit_loop(vm::chunk::difference_type pos);

	uint16_t identifier_constant(const scanning::token& identifier);

	void set_constant(vm::full_opcode_type pos, const vm::value& val);

	uint16_t make_constant(const vm::value& val);

	vm::full_opcode_type make_function(const std::shared_ptr<parsing::statement>& func);

	// To make it right for scope_begin(). because the first scope should have depth 0
	int64_t current_scope_depth_{ -1 };

	int64_t local_totals_{ 1 }; // first slot is in use

	std::vector<std::unique_ptr<local_scope>> local_scopes_{};

	std::vector<vm::function_object_raw_pointer> functions_{};

	std::unordered_map<std::shared_ptr<parsing::statement>, vm::full_opcode_type> functions_ids_{};

	size_t function_id_counter_{ 1 };

	std::shared_ptr<vm::object_heap> heap_{};

	std::shared_ptr<resolving::binding_table> bindings_{};
};

}