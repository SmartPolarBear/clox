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

#include <logger/logger.h>

#include <gtest/gtest.h>

#include <cstring>

class ExpressionTest : public ::testing::Test
{

protected:


	virtual void SetUp()
	{

	}

	virtual void TearDown()
	{

	}


	const char* comma_{
#include <expression/comma.txt>
	};

	const char* comma_out_{
#include <expression/comma.out>
	};


	const char* pre_post_fix_{
#include <expression/pre_post_fix.txt>
	};

	const char* pre_post_fix_out_{
#include <expression/pre_post_fix.out>
	};


	const char* simple_{
#include <expression/simple.txt>
	};

	const char* simple_out_{
#include <expression/simple.out>
	};

	const char* ternary_{
#include <expression/ternary.txt>
	};

	const char* ternary_out_{
#include <expression/ternary.out>
	};


};

#include <driver/driver.h>

using namespace std;

using namespace clox::driver;


TEST_F(ExpressionTest, CommaTest)
{
	test_scaffold_console cons{};

	int ret = run_code(cons, comma_);
	ASSERT_EQ(ret, 0);

	auto output = cons.get_written_text();
	ASSERT_NE(output.find(comma_out_), string::npos);
}

TEST_F(ExpressionTest, PrePostfixTest)
{
	test_scaffold_console cons{};

	int ret = run_code(cons, pre_post_fix_);
	ASSERT_EQ(ret, 0);

	auto output = cons.get_written_text();
	ASSERT_NE(output.find(pre_post_fix_out_), string::npos);
}

TEST_F(ExpressionTest, SimpleTest)
{
	test_scaffold_console cons{};

	int ret = run_code(cons, simple_);
	ASSERT_EQ(ret, 0);

	auto output = cons.get_written_text();
	ASSERT_NE(output.find(simple_out_), string::npos);
}


TEST_F(ExpressionTest, TernaryTest)
{
	test_scaffold_console cons{};

	int ret = run_code(cons, ternary_);
	ASSERT_EQ(ret, 0);

	auto output = cons.get_written_text();
	ASSERT_NE(output.find(ternary_out_), string::npos);
}


