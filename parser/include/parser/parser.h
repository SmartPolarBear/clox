
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

#pragma once

#include <parser/gen/parser_classes.inc>
#include <parser/parser_error.h>
#include <scanner/nil_value.h>

#include <helper/parameter_pack.h>

#include <concepts>
#include <memory>
#include <vector>
#include <ranges>
#include <algorithm>
#include <tuple>

namespace clox::parsing
{


class parser final
{
public:
	using token = scanning::token;

	static constexpr size_t FUNC_ARG_LIST_MAX = 1024;

	[[nodiscard]] explicit parser(std::vector<scanning::token>&& tokens)
			: tokens_{ tokens }
	{
	}

	std::vector<std::shared_ptr<statement>> parse();


private:
	/// statement -> exprStmt
	///               | ifStmt
	///               | printStmt
	///				  | returnStmt
	///	              | whileStmt
	///               | block ;
	/// \return
	std::shared_ptr<statement> stmt();

	std::shared_ptr<statement> print_stmt();

	std::shared_ptr<statement> expr_stmt();

	/// whileStmt -> "while" "(" expression ")" statement ;
	/// \return
	std::shared_ptr<statement> while_stmt();

	/// ifStmt -> "if" "(" expression ")" statement
	///               ( "else" statement )? ;
	/// \return
	std::shared_ptr<statement> if_stmt();

	/// forStmt -> "for" "(" ( varDecl | exprStmt | ";" )
	///                 expression? ";"
	///                 expression? ")" statement ;
	/// \return
	std::shared_ptr<statement> for_stmt();

	/// returnStmt -> "return" expression? ";" ;
	/// \return
	std::shared_ptr<statement> return_stmt();

	/// expr -> comma
	/// \return
	std::shared_ptr<expression> expr();

	/// comma ->  conditional ( "," conditional )* ;
	/// \return
	std::shared_ptr<expression> comma();

	/// conditional -> assigment ("?" expr ":" conditional )? ;
	/// \return
	std::shared_ptr<expression> conditional();

	/// initializer -> conditional (not expr because comma expression will lead to ambiguity
	///               | '{' initializer_list '}'
	///			      | '{' initializer_list ',' '}'
	/// \return
	std::shared_ptr<expression> initializer_expr();

	/// initializer_list -> initializer (',' initializer)*
	///
	/// \return
	std::shared_ptr<expression> initializer_list_expr();

	/// assignment -> IDENTIFIER "=" assignment
	///               | logical_or
	/// \return
	std::shared_ptr<expression> assigment();

	/// logical_or -> logical_and ("or" logical_and)*
	/// \return
	std::shared_ptr<expression> logical_or();

	/// logical_and -> equality ("and" equality)*
	/// \return
	std::shared_ptr<expression> logical_and();

	/// equality -> comparison ( ( "!=" | "==" ) comparison )*
	/// \return
	std::shared_ptr<expression> equality();

	/// comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )*
	/// \return
	std::shared_ptr<expression> comparison();

	/// term -> factor ( ( "-" | "+" ) factor )*
	/// \return
	std::shared_ptr<expression> term();

	/// factor -> unary ( ( "/" | "*" ) unary )*
	/// \return
	std::shared_ptr<expression> factor();

	/// unary -> ( "!" | "-" ) unary | exponent;
	/// \return
	std::shared_ptr<expression> unary();

	/// exponent -> (prefix "**" unary)
	///            | prefix ;
	/// \return
	std::shared_ptr<expression> exponent();

	/// prefix -> ("++" | "--") primary
	///            | postfix
	/// \return
	std::shared_ptr<expression> prefix();

	/// postfix -> primary ("--"|"++")* | call ;
	/// \return
	std::shared_ptr<expression> postfix();

	/// call -> primary ( "(" arguments? ")" )* ;
	/// arguments      → expression ( "," expression )* ;
	/// \return
	std::shared_ptr<expression> call();

	///primary        → "true" | "false" | "nil" | "this"
	///               | NUMBER | STRING | IDENTIFIER | "(" expression ")"
	///               | "base" "." IDENTIFIER ;
	/// \return
	std::shared_ptr<expression> primary();

	/// declaration -> classDecl
	/// 			  | funDecl
	///               | varDecl
	///               | statement ;
	/// \return
	std::shared_ptr<statement> declaration();

	/// classDecl -> "class" IDENTIFIER "{" class_member* "}" ;
	/// \return
	std::shared_ptr<statement> class_declaration();

	/// classMember -> funDecl | constructorDecl | var_decl
	/// \return tuple{leading token type,statement}
	std::tuple<scanning::token_type, std::shared_ptr<statement>> class_member();


	/// funDecl -> "fun" function ;
	/// function -> IDENTIFIER "(" parameters? ")" ":" typeExpr  block ;
	/// parameter ->  IDENTIFIER ":" typeExpr
	/// parameters -> parameter ( "," parameter )* ;
	/// \return
	std::shared_ptr<statement> func_declaration(function_statement_type type);

	scanning::token func_declaration_name(function_statement_type type);


	/// var_decl -> "var" IDENTIFIER (":" typeExpr)? ("=" initializer)? ";"
	/// \return
	std::shared_ptr<statement> var_declaration();

	/// type_expr -> non_union_type ;
	/// \return
	std::shared_ptr<type_expression> type_expr();

	/// non_union_type -> callable_type | variable_type ("[" array_len "]")
	/// array_len -> expr
	/// \return
	std::shared_ptr<type_expression> non_union_type();

	/// callable_type -> "fun"  "(" parameter_types? ")" ":" typeExpr
	/// parameter_types -> type_expr *
	/// \return
	std::shared_ptr<type_expression> callable_type();

	/// variable_type -> IDENTIFIER
	/// \return
	std::shared_ptr<type_expression> variable_type();


	/// block -> "{" declaration* "}" ;
	/// \return
	std::vector<std::shared_ptr<statement>> block();

private:
	/// finish parsing argument list and things
	/// \return
	std::shared_ptr<expression> call_finish_parse(const std::shared_ptr<expression>& expr);

	void synchronize();

	token consume(scanning::token_type t, const std::string& msg);

	token consume(std::initializer_list<scanning::token_type> tks, const std::string& msg);

	parse_error error(token t, const std::string& msg);

	bool match(std::initializer_list<scanning::token_type> types, int64_t next)
	{
		for (const auto& t:types)
		{
			if (check(t, next))
			{
				[[maybe_unused]]auto _ = advance();
				return true;
			}
		}

		return false;
	}

	bool match(std::initializer_list<scanning::token_type> types)
	{
		return match(types, 0);
	}

	[[nodiscard]] bool check(scanning::token_type t, int64_t next)
	{
		if (is_end())return false;
		return peek(next).type() == t;
	}

	[[nodiscard]] bool check(scanning::token_type t)
	{
		return check(t, 0);
	}

	[[nodiscard]]token peek(int64_t offset)
	{
		return tokens_.at(cur_ + offset);
	}

	[[nodiscard]]token peek()
	{
		return peek(0);
	}

	[[nodiscard]]token advance()
	{
		if (!is_end())cur_++;
		return previous();
	}

	[[nodiscard]]bool is_end()
	{
		return peek().type() == scanning::token_type::FEND;
	}

	[[nodiscard]]token previous()
	{
		return tokens_.at(cur_ - 1);
	}

	std::vector<token> tokens_;

	size_t cur_{ 0 };
};

}

