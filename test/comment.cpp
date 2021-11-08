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
#include <test_interpreter_adapter.h>

#include <logger/logger.h>

#include <gtest/gtest.h>

#include <cstring>


class CommentTest : public ::testing::Test
{

protected:
	virtual void SetUp() override
	{
		clox::logging::logger::instance().clear_error();

		assert(strlen(good_comment_) != 0);
		assert(strlen(bad_comment_) != 0);
	}

	virtual void TearDown()
	{
		clox::logging::logger::instance().clear_error();
	}


	[[nodiscard]] std::string bad_comment_code() const
	{
		return bad_comment_;
	}

	[[nodiscard]] std::string good_comment_code() const
	{
		return good_comment_;
	}

private:
	const char* bad_comment_{
#include "comment/bad_comment.txt"
	};

	const char* good_comment_{
#include "comment/comment.txt"
	};

};

#include <driver/run.h>


using namespace std;

using namespace clox::driver;

TEST_F(CommentTest, GoodCommentTest)
{
	test_scaffold_console cons{};

	int ret = run_code(cons,test_interpreter_adapater::get(cons), good_comment_code());
	ASSERT_EQ(ret, 0);

	auto output = cons.get_written_text();
	ASSERT_NE(output.find("fuck"), string::npos);

	// count how many f-words are there.
	auto find = output.find("fuck");
	auto count = 1;
	while ((find = output.find("fuck", find + 1)) != string::npos)
	{
		count++;
	}

	// we should have 2 f-words
	ASSERT_EQ(count, 2);
}


TEST_F(CommentTest, BadCommentTest)
{
	test_scaffold_console cons{};

	int ret = run_code(cons, test_interpreter_adapater::get(cons), bad_comment_code());
	ASSERT_NE(ret, 0);

	auto output = cons.get_error_text();
	ASSERT_NE(output.find("Error: Unclosed block comment found."), string::npos);
}
