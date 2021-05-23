#include <scanner.h>
#include <logger.h>

#include <format>

using namespace std;
using namespace clox::scanner;
using namespace clox::logger;

std::vector<token> clox::scanner::scanner::scan()
{
	return std::vector<token>();
}


void scanner::scan_next_token()
{
	char c = advance();
	switch (c)
	{
	case '(':
		add_token(token_type::LEFT_PAREN);
		break;
	case ')':
		add_token(token_type::RIGHT_PAREN);
		break;
	case '{':
		add_token(token_type::LEFT_BRACE);
		break;
	case '}':
		add_token(token_type::RIGHT_BRACE);
		break;
	case ',':
		add_token(token_type::COMMA);
		break;
	case '.':
		add_token(token_type::DOT);
		break;
	case '-':
		add_token(token_type::MINUS);
		break;
	case '+':
		add_token(token_type::PLUS);
		break;
	case ';':
		add_token(token_type::SEMICOLON);
		break;
	case '*':
		add_token(token_type::STAR);
		break;

	case '!':
		add_token(match('=') ? token_type::BANG_EQUAL : token_type::BANG);
		break;
	case '=':
		add_token(match('=') ? token_type::EQUAL_EQUAL : token_type::EQUAL);
		break;
	case '<':
		add_token(match('=') ? token_type::LESS_EQUAL : token_type::LESS);
		break;
	case '>':
		add_token(match('=') ? token_type::GREATER_EQUAL : token_type::GREATER);
		break;

	case '/':
		if (match('/'))
		{
			// this is a comment
			while (peek() != '\n' && !is_end())advance();
		}
		else
		{
			add_token(token_type::SLASH);
		}
		break;

	case ' ':
	case '\r':
	case '\t':
		// Do nothing to ignore whitespaces.
		break;

	case '\n':
		line_++;
		break;

	default:
		logger::logger::instance().error(line_, format("Unexpected character {}.", c));
		break;
	}
}

void scanner::add_token(token_type t)
{
	add_token(t, token::empty_literal);
}

void scanner::add_token(token_type t, const std::any& literal)
{
	auto text = src_.substr(start_, cur_ - start_ + 1);
	tokens_.emplace_back(t, text, literal, line_);
}

char scanner::advance()
{
	return src_.at(cur_++);
}

char scanner::peek()
{
	if (is_end())return 0;

	return src_.at(cur_);
}

bool scanner::match(char expect)
{
	if (is_end())return false;
	if (src_.at(cur_) != expect)return false;

	cur_++;
	return true;
}
