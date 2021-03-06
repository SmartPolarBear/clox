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
// Created by cleve on 9/18/2021.
//
#pragma once

#include <base/base.h>

namespace clox::base
{
/// \brief runtime_predefined_configuration convert predefine macros to const expressions to be used with constexpr if
class runtime_predefined_configuration final
		: public predefined_configuration<runtime_predefined_configuration>
{
public:
	runtime_predefined_configuration() = default;

#ifndef DEBUG_STRESS_GC
#warning "DEBUG_STRESS_GC is defined to 0 by default"
#define DEBUG_STRESS_GC 0
#endif

	/// \brief call gc everytime when a block of memory is allocated
	static inline constexpr bool ENABLE_DEBUG_STRESS_GC = DEBUG_STRESS_GC;

#ifndef DEBUG_LOGGING_GC
#warning "DEBUG_LOGGING_GC is defined to 0 by default"
#define DEBUG_LOGGING_GC 0
#endif

	static inline constexpr bool ENABLE_DEBUG_LOGGING_GC = DEBUG_LOGGING_GC;
};

}