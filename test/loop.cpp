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

class LoopTest : public ::testing::Test
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

	const char* for_{
#include <loop/for.txt>
	};

	const char* for_out_{
#include <loop/for.out>
	};


	const char* while_{
#include <loop/while.txt>
	};

	const char* while_out_{
#include <loop/while.out>
	};
};


#include <driver/run.h>


using namespace std;

using namespace clox::driver;

TEST_F(LoopTest, ForTest)
{
	test_scaffold_console cons{};

	int ret = run_code(cons,test_interpreter_adapater::get(cons), for_);
	ASSERT_EQ(ret, 0);

	auto output = cons.get_written_text();
	ASSERT_NE(output.find(for_out_), string::npos);
}

TEST_F(LoopTest, WhileTest)
{
	test_scaffold_console cons{};

	int ret = run_code(cons,test_interpreter_adapater::get(cons), while_);
	ASSERT_EQ(ret, 0);

	auto output = cons.get_written_text();
	ASSERT_NE(output.find(while_out_), string::npos);
}
