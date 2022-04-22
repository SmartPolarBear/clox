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
// Created by cleve on 9/6/2021.
//

#pragma once


#include "scanner/scanner.h"

#include "object.h"

#include <variant>
#include <string>

#include <memory>
#include <unordered_set>


namespace clox::interpreting::vm
{

using string_object_raw_pointer = class string_object*;

class string_object :
		public object
{
public:
	friend class garbage_collector;
	friend class object_heap;

	struct string_object_intern_hash
	{
		auto operator()(string_object_raw_pointer obj) const noexcept
		{
			return std::hash<std::string>()(obj->string());
		}

		auto operator()(const std::string& obj) const noexcept
		{
			return std::hash<std::string>()(obj);
		}
	};

	using string_interns_table_type = std::unordered_set<string_object_raw_pointer, string_object_intern_hash>;


	string_object() = delete;

	[[nodiscard]] object_type type() const noexcept override;

protected:


	void blacken(struct garbage_collector* gc_inst) override;

public:
	static string_object_raw_pointer
	create_on_heap(const std::shared_ptr<class object_heap>& heap, const std::string& value);

	[[nodiscard]] std::string string() const;

	std::string printable_string() override;

private:
	explicit string_object(std::string value);

	std::string data_{};

	static string_interns_table_type interns_;
};

}
