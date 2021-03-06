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
// Created by cleve on 6/19/2021.
//

#include <interpreter/classic/environment.h>

#include <utility>
#include <format>

using namespace std;
using namespace clox::interpreting;
using namespace clox::interpreting::classic;

std::optional<evaluating_result> classic::environment::get(const clox::scanning::token& name)
{
	if (values_->contains(name.lexeme()))
	{
		return values_->at(name.lexeme());
	}
	else if (funcs_->contains(name.lexeme()))
	{
		return funcs_->at(name.lexeme());
	}

	if (auto pa = parent_.lock())
	{
		return pa->get(name);
	}

	return nullopt;
}

void classic::environment::put(const std::string& name, evaluating_result value)
{
	(*values_)[name] = std::move(value);
}

void classic::environment::assign(const clox::scanning::token& name, evaluating_result val)
{
	if (values_->contains(name.lexeme()))
	{
		(*values_)[name.lexeme()] = std::move(val);
		return;
	}

	if (auto pa = parent_.lock())
	{
		pa->assign(name, val);
		return;
	}

	throw runtime_error{ name, std::format("Undefined variable '{}'.", name.lexeme()) };
}

std::optional<evaluating_result> classic::environment::get_at(const string& name, int64_t depth)
{
	auto an = ancestor(depth);
	if (auto p = an.lock())
	{
		if (p->values_->contains(name))
		{
			return p->values_->at(name);
		}
		else if (p->funcs_->contains(name))
		{
			return p->funcs_->at(name);
		}

		return std::nullopt;


	}
	return std::nullopt;
}

std::weak_ptr<classic::environment> classic::environment::ancestor(int64_t dist)
{
	weak_ptr<environment> ret{ this->shared_from_this() };
	for (int i = 0; i < dist; i++)
	{
		if (auto p = ret.lock())
		{
			ret = p->parent_;
		}
		else
		{
			throw std::runtime_error{ "Internal error" };
		}
	}
	return ret;
}

void classic::environment::assign_at(const clox::scanning::token& name, evaluating_result val, int64_t depth)
{
	auto an = ancestor(depth);
	if (auto p = an.lock())
	{
		(*(p->values_))[name.lexeme()] = std::move(val);
	}
	else
	{
		throw std::runtime_error{ "Internal error" };
	}
}

void classic::environment::put(const string& name, const shared_ptr<parsing::statement>& stmt,
		const shared_ptr<struct callable>& func)
{
	(*funcs_)[name][stmt] = func;
}
