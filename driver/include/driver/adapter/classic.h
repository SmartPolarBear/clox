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
// Created by cleve on 9/10/2021.
//

#pragma once

#include "helper/console.h"

#include "resolver/resolver.h"
#include "interpreter/classic/interpreter.h"

#include "driver/interpreter_adapter.h"

namespace clox::driver
{
class classic_interpreter_adapter final
		: public interpreter_adapter
{
public:
	explicit classic_interpreter_adapter(helper::console& cons)
			: cons_(&cons),
			  repl_resolver_(),
			  repl_intp_(cons)
	{
	}


	int full_code(const std::vector<std::shared_ptr<parsing::statement>>& code) override;

	int repl(const std::vector<std::shared_ptr<parsing::statement>>& code) override;

private:
	resolving::resolver repl_resolver_{};
	interpreting::classic::interpreter repl_intp_;

	mutable helper::console* cons_{};
};
}
