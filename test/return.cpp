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
// Created by cleve on 7/11/2021.
//

#include <test_scaffold_console.h>

#include <gtest/gtest.h>

#include <cstring>

class ReturnTest : public ::testing::Test
{

protected:
	virtual void SetUp()
	{
		assert(strlen(return_code_) != 0);
		assert(strlen(bad_return_code_) != 0);
	}

	[[nodiscard]] std::string return_code()
	{
		return return_code_;
	}

	[[nodiscard]] std::string bad_return_code()
	{
		return bad_return_code_;
	}

private:
	const char* return_code_{
#include "return/return.txt"
	};

	const char* bad_return_code_{
#include "return/bad_return.txt"
	};
};

#include <driver/driver.h>

using namespace std;

using namespace clox::driver;

TEST_F(ReturnTest, GoodReturnTest)
{
	test_scaffold_console cons{};

	int ret = run(cons, return_code());
	ASSERT_EQ(ret, 0);

	auto output = cons.get_written_text();
	ASSERT_EQ(output, "fuck\n");
}

TEST_F(ReturnTest, BadReturnTest)
{
	test_scaffold_console cons{};

	int ret = run(cons, bad_return_code());
	ASSERT_NE(ret, 0);

	auto output = cons.get_written_text();

	// TODO: make all error message a string constant.
	ASSERT_NE(output.find("Return statement in none-function scoop."), string::npos);
}