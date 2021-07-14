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
// Created by cleve on 7/12/2021.
//

#include <test_scaffold_console.h>

#include <logger/logger.h>

#include <gtest/gtest.h>

#include <cstring>


class ScoopTest : public ::testing::Test
{
protected:

	virtual void SetUp()
	{
		clox::logging::logger::instance().clear_error();

		assert(strlen(bind_) != 0);
		assert(strlen(closure_) != 0);
		assert(strlen(scoop_) != 0);

	}

	virtual void TearDown()
	{
		clox::logging::logger::instance().clear_error();
	}

	const char* bind_{
#include <scoop/bind.txt>
	};

	const char* closure_{
#include <scoop/closure.txt>
	};

	const char* closure_out_{
#include <scoop/closure.out>
	};

	const char* scoop_{
#include <scoop/scoop.txt>
	};

	const char* scoop_out_{
#include <scoop/scoop.out>
	};
};

#include <driver/driver.h>

using namespace std;

using namespace clox::driver;


TEST_F(ScoopTest, BindTest)
{
	test_scaffold_console cons{};

	int ret = run(cons, bind_);
	ASSERT_EQ(ret, 0);

	auto output = cons.get_written_text();
	ASSERT_NE(output.find("global\nlocal"), string::npos);
}

TEST_F(ScoopTest, ClosureTest)
{
	test_scaffold_console cons{};

	int ret = run(cons, closure_);
	ASSERT_EQ(ret, 0);

	auto output = cons.get_written_text();
	ASSERT_NE(output.find(closure_out_), string::npos);
}

TEST_F(ScoopTest, ScoopTest)
{
	test_scaffold_console cons{};

	int ret = run(cons, scoop_);
	ASSERT_EQ(ret, 0);

	auto output = cons.get_written_text();
	ASSERT_NE(output.find(scoop_out_), string::npos);
}