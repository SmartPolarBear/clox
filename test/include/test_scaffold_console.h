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

#include <helper/console.h>

#include <sstream>
#include <string>

class test_scaffold_console final
		: public clox::helper::console
{
public:
	test_scaffold_console() = default;

	explicit test_scaffold_console(const std::string& in_str) : in_ss_(in_str)
	{
	}

	void write(const std::string& str) override;

	void write(std::string_view sv) override;

	std::ostream& out() override;

	[[nodiscard]] std::string get_written_text() const;

	[[nodiscard]] std::string get_error_text() const;

	[[nodiscard]] std::string get_log_text() const;

	std::ostream& error() override;

	std::string read() override;

	std::optional<std::string> read_line() override;

	void write_line(const std::string& str) override;

	void write_line(std::string_view sv) override;

	std::istream& in() override;

	std::ostream& log() override;

private:
	std::stringstream out_ss_{};
	std::stringstream in_ss_{};
	std::stringstream error_ss_{};
	std::stringstream log_ss_{};

};