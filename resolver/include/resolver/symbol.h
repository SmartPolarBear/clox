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
// Created by cleve on 8/4/2021.
//

#pragma once

#include <scanner/scanner.h>

#include <parser/parser.h>

#include <resolver/lox_type.h>

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <unordered_map>
#include <compare>
#include <stdexcept>

namespace clox::resolving
{

enum class symbol_type
{
	ST_NAMED = 1,
	ST_FUNCTION,
	ST_UPVALUE,
};

template<typename T>
struct symbol_tag;

class symbol
{
public:
	[[nodiscard]] virtual symbol_type symbol_type() const = 0;

	[[nodiscard]] virtual std::shared_ptr<lox_type> type() const = 0;
};

// TODO: add function_symbol or something likely

class function_multi_symbol final
		: public symbol
{
public:
	[[nodiscard]] resolving::symbol_type symbol_type() const override;

	[[nodiscard]] std::shared_ptr<lox_type> type() const override;
};

template<>
struct symbol_tag<function_multi_symbol>
{
	static constexpr symbol_type type = symbol_type::ST_FUNCTION;
};

class named_symbol final
		: public symbol
{
public:
	enum class named_symbol_type
	{
		GLOBAL, LOCAL
	};
public:
	named_symbol() = default;

	~named_symbol() = default;

	/// Create a named symbol, it is by default an global
	/// \param name
	/// \param type
	[[nodiscard]] explicit named_symbol(std::string name, std::shared_ptr<lox_type> type);

	[[nodiscard]] explicit named_symbol(std::string name, std::shared_ptr<lox_type> type,
			named_symbol_type t, int64_t slot_index);

	[[nodiscard]] resolving::symbol_type symbol_type() const override;

	[[nodiscard]] std::shared_ptr<lox_type> type() const override;

	[[nodiscard]] bool is_global() const
	{
		return symbol_type_ == named_symbol_type::GLOBAL;
	}

	[[nodiscard]] bool is_local() const
	{
		return symbol_type_ == named_symbol_type::LOCAL;
	}

	[[nodiscard]] bool is_captured() const
	{
		return is_captured_;
	}

	void capture()
	{
		is_captured_ = true;
	}


	[[nodiscard]] int64_t slot_index() const
	{
		return slot_index_;
	}

private:
	std::string name_{};
	std::shared_ptr<lox_type> type_{};

	named_symbol_type symbol_type_{ named_symbol_type::GLOBAL };

	bool is_captured_{ false };

	int64_t slot_index_{ -1 };
};

template<>
struct symbol_tag<named_symbol>
{
	static constexpr symbol_type type = symbol_type::ST_NAMED;
};


template<typename T>
[[maybe_unused, nodiscard]] static inline std::shared_ptr<T>
downcast_symbol(const std::shared_ptr<symbol>& symbol)
{
	if (symbol->symbol_type() == symbol_tag<T>::type) [[likely]]
	{
		return std::static_pointer_cast<T>(symbol);
	}
	else [[unlikely]]
	{
		return nullptr;
	}
}


}