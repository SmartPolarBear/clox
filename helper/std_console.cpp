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

#include <helper/std_console.h>

using namespace std;

void clox::helper::std_console::write(const std::string& str)
{
	out() << str;
}

void clox::helper::std_console::write(std::string_view sv)
{
	out() << sv;
}

void clox::helper::std_console::write_line(const std::string& str)
{
	out() << str << endl;
}

void clox::helper::std_console::write_line(std::string_view sv)
{
	out() << sv << endl;
}


std::ostream& clox::helper::std_console::out()
{
	return *out_stream_;
}

std::string clox::helper::std_console::read()
{
	std::string ret{};
	in() >> ret;
	return ret;
}

std::optional<std::string> clox::helper::std_console::read_line()
{
	std::string line{};
	if (std::getline(in(), line))
	{
		return line;
	}
	else
	{
		return std::nullopt;
	}
}


std::istream& clox::helper::std_console::in()
{
	return std::cin;
}

ostream& clox::helper::std_console::error()
{
	return std::cerr;
}

ostream& clox::helper::std_console::log()
{
	return std::clog;
}
