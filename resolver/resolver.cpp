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

#include <resolver/resolver.h>

#include <logger/logger.h>

#include <memory>
#include <format>
#include <ranges>

#include <gsl/gsl>

using namespace clox::parsing;
using namespace clox::logging;
using namespace clox::resolver;
using namespace clox::interpreting;

using namespace std;
using namespace gsl;

void resolver::visit_assignment_expression(const std::shared_ptr<parsing::assignment_expression>& e)
{
	resolve(e->get_value());
	resolve_local(e, e->get_name());
}

void resolver::visit_binary_expression(const std::shared_ptr<parsing::binary_expression>& expr)
{
	resolve(expr->get_left());
	resolve(expr->get_right());
}

void resolver::visit_unary_expression(const std::shared_ptr<parsing::unary_expression>& ue)
{
	resolve(ue->get_right());
}

void resolver::visit_literal_expression(const std::shared_ptr<parsing::literal_expression>& ptr)
{
	// Do nothing for this
}

void resolver::visit_grouping_expression(const std::shared_ptr<parsing::grouping_expression>& ge)
{
	resolve(ge->get_expr());
}

void resolver::visit_var_expression(const std::shared_ptr<parsing::var_expression>& ve)
{
	if (!scopes_.empty())
	{
		if (auto existence = scope_top_find(ve->get_name().lexeme());existence.has_value() &&
																	 !existence.value())
		{
			logger::instance().error(ve->get_name(),
					std::format("Reading {} before its initialization.", ve->get_name().lexeme()));
		}
	}

	resolve_local(ve, ve->get_name());
}

void resolver::visit_ternary_expression(const std::shared_ptr<parsing::ternary_expression>& te)
{
	resolve(te->get_cond());
	resolve(te->get_true_expr());
	resolve(te->get_false_expr());
}

void resolver::visit_logical_expression(const std::shared_ptr<parsing::logical_expression>& le)
{
	resolve(le->get_left());
	resolve(le->get_right());
}

void resolver::visit_call_expression(const std::shared_ptr<parsing::call_expression>& ce)
{
	resolve(ce->get_calle());

	for (const auto& arg:ce->get_args())
	{
		resolve(arg);
	}
}

void resolver::visit_expression_statement(const std::shared_ptr<parsing::expression_statement>& es)
{
	resolve(es->get_expr());
}

void resolver::visit_print_statement(const std::shared_ptr<parsing::print_statement>& pe)
{
	resolve(pe->get_expr());
}

void resolver::visit_variable_statement(const std::shared_ptr<parsing::variable_statement>& stmt)
{
	declare(stmt->get_name());
	auto _ = finally([this, &stmt]
	{
		this->define(stmt->get_name());
	});

	if (stmt->get_initializer())
	{
		resolve(stmt->get_initializer());
	}
}

void resolver::visit_block_statement(const std::shared_ptr<parsing::block_statement>& blk)
{
	scope_begin();
	auto _ = finally([this]
	{
		this->scope_end();
	});

	resolve(blk->get_stmts());
}

void resolver::visit_while_statement(const std::shared_ptr<parsing::while_statement>& ws)
{
	resolve(ws->get_cond());
	resolve(ws->get_body());
}

void resolver::visit_if_statement(const std::shared_ptr<parsing::if_statement>& stmt)
{
	resolve(stmt->get_cond());
	resolve(stmt->get_true_stmt());
	if (stmt->get_false_stmt())resolve(stmt->get_false_stmt());
}

void resolver::visit_function_statement(const std::shared_ptr<parsing::function_statement>& stmt)
{
	declare(stmt->get_name());
	define(stmt->get_name());

	resolve_function(stmt, function_type::FT_FUNCTION);
}

void resolver::visit_return_statement(const std::shared_ptr<parsing::return_statement>& rs)
{
	if (cur_func_ == function_type::FT_NONE)
	{
		logger::instance().error(rs->get_return_keyword(), "Return statement in none-function scoop.");
	}

	if (rs->get_val())
	{
		resolve(rs->get_val());
	}
}

void resolver::resolve(const std::vector<std::shared_ptr<parsing::statement>>& stmts)
{
	for (const auto& stmt:stmts)
	{
		resolve(stmt);
	}
}

void resolver::resolve(const std::shared_ptr<parsing::statement>& stmt)
{
	accept(*stmt, *this);
}

void resolver::resolve(const std::shared_ptr<clox::parsing::expression>& expr)
{
	accept(*expr, *this);
}

void resolver::scope_begin()
{
	scope_push(make_shared<unordered_map<string, bool>>());
}

void resolver::scope_end()
{
	scope_pop();
}

void resolver::declare(const clox::scanning::token& t)
{
	if (scopes_.empty())return;

	auto top = scope_top();

	if (top->contains(t.lexeme()))
	{
		logger::instance().error(t, std::format("{} already exists in this scoop.", t.lexeme()));
	}

	(*top)[t.lexeme()] = false;
}

void resolver::define(const clox::scanning::token& t)
{
	if (scopes_.empty())return;

	(*scope_top())[t.lexeme()] = true;

}

void resolver::resolve_local(const shared_ptr<parsing::expression>& expr, const clox::scanning::token& tk)
{
	int64_t depth = 0;

	for (const auto& s:scopes_ | views::reverse) // traverse from the stack top, which has a depth of zero.
	{
		if (s->contains(tk.lexeme()))
		{
			intp_->resolve(expr, depth);
			return;
		}
		depth++;
	}
}

void resolver::resolve_function(const shared_ptr<parsing::function_statement>& func, function_type type)
{
	auto parent_type = cur_func_;
	cur_func_ = type;
	scope_begin();

	auto _ = finally([this, parent_type]
	{
		this->scope_end();
		cur_func_ = parent_type;
	});

	for (const auto& tk:func->get_params())
	{
		declare(tk);
		define(tk);
	}

	resolve(func->get_body());
}

void resolver::visit_postfix_expression(const std::shared_ptr<parsing::postfix_expression>& pe)
{
	resolve(pe->get_left());
}

void resolver::visit_class_statement(const std::shared_ptr<class_statement>& cls)
{
	declare(cls->get_name());
	define(cls->get_name());

	for (const auto& method:cls->get_methods())
	{
		auto decl = function_type::FT_METHOD;
		resolve_function(method, decl);
	}
}

void resolver::visit_get_expression(const std::shared_ptr<get_expression>& ptr)
{
	resolve(ptr->get_object());
}

void resolver::visit_set_expression(const std::shared_ptr<set_expression>& se)
{
	resolve(se->get_val());
	resolve(se->get_object());
}
