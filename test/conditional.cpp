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
// Created by cleve on 7/15/2021.
//


#include <test_scaffold_console.h>
#include <test_interpreter_adapter.h>

#include <logger/logger.h>

#include <gtest/gtest.h>

#include <cstring>

class ConditionalTest : public ::testing::Test
{

protected:

	virtual void SetUp()
	{
		clox::logging::logger::instance().clear_error();

	}

	virtual void TearDown()
	{
		clox::logging::logger::instance().clear_error();

	}

	const char* if_{
#include <conditional/if.txt>
	};

	const char* if_out_{
#include <conditional/if.out>
	};

};


#include <driver/run.h>


using namespace std;

using namespace clox::driver;


TEST_F(ConditionalTest, IfTest)
{
	test_scaffold_console cons{};

	int ret = run_code(cons,test_interpreter_adapater::get(cons), if_);
	ASSERT_EQ(ret, 0);

	auto output = cons.get_written_text();
	ASSERT_NE(output.find(if_out_), string::npos);
}
