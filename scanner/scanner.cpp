#include <scanner/scanner.h>
#include <logger/logger.h>

#include <format>
#include <ranges>

using namespace std;

using namespace clox::scanning;
using namespace clox::logging;

bool validator::valid_number_literal_component(char c)
{
	return isdigit(c);
}

bool validator::valid_identifier_component(char c)
{
	return isdigit(c) || isalpha(c);
}

bool validator::isdigit(char c)
{
	return c >= '0' && c <= '9';
}

bool validator::isalpha(char c)
{
	return (c >= 'a' && c <= 'z') ||
		   (c >= 'A' && c <= 'Z') ||
		   c == '_';
}


std::vector<token> clox::scanning::scanner::scan()
{
	while (!is_end())
	{
		start_ = cur_;
		scan_next_token();
	}

	tokens_.emplace_back(token_type::FEND, "", token::empty_literal, line_);
	return tokens_;
}


void scanner::add_token(token_type t)
{
	add_token(t, token::empty_literal);
}

void scanner::add_token(token_type t, const literal_value_type& literal)
{
	tokens_.emplace_back(t, whole_lexeme(), literal, line_);
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

char scanner::peek(size_t n)
{
	if (is_end())return 0;
	if (cur_ + n >= src_.size())return 0;

	return src_.at(cur_ + n);
}


bool scanner::match(char expect)
{
	if (is_end())return false;
	if (src_.at(cur_) != expect)return false;

	cur_++;
	return true;
}

void scanner::scan_string()
{
	while (peek() != '"' && !is_end())
	{
		if (peek() == '\n')line_++;
		advance();
	}

	if (is_end())
	{
		logging::logger::instance().error(line_, "Unterminated string.");
		return;
	}

	advance(); // eat the closing "

	string val = src_.substr(start_ + 1, cur_ - start_ - 2);

	add_token(token_type::STRING, val);
}

void scanner::scan_number_literal()
{
	while (validator::valid_number_literal_component(peek()))advance();

	bool floating{ false };
	if (peek() == '.' && validator::valid_number_literal_component(peek(1)))
	{
		floating = true;
		advance();
		while (validator::valid_number_literal_component(peek()))advance();
	}

	if (floating)
	{
		add_token(token_type::FLOATING, std::stold(string{ whole_lexeme() }));
	}
	else
	{
		add_token(token_type::INTEGER, std::stoll(string{ whole_lexeme() }));
	}
}

std::string scanner::whole_lexeme()
{
	return src_.substr(start_, cur_ - start_);
}

void scanner::scan_identifier()
{
	while (validator::valid_identifier_component(peek()))advance();

	auto text = whole_lexeme();
	auto keyword = keywords_to_type_.find(text);

	if (keyword != keywords_to_type_.end())add_token(keywords_to_type_[text]);
	else add_token(token_type::IDENTIFIER);
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
	case '[':
		add_token(token_type::LEFT_BRACKET);
		break;
	case ']':
		add_token(token_type::RIGHT_BRACKET);
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
		if (match('-'))
		{
			add_token(token_type::MINUS_MINUS);
		}
		else if (match('>'))
		{
			add_token(token_type::ARROW);
		}
		else
		{
			add_token(token_type::MINUS);
		}
		break;
	case '+':
		if (match('+'))
		{
			add_token(token_type::PLUS_PLUS);
		}
		else
		{
			add_token(token_type::PLUS);
		}
		break;
	case ';':
		add_token(token_type::SEMICOLON);
		break;
	case '*':
		if (match('*'))
		{
			add_token(token_type::STAR);
		}
		else
		{
			add_token(token_type::STAR);
		}
		break;
	case '?':
		add_token(token_type::QMARK);
		break;
	case '|':
		add_token(token_type::PIPE);
		break;
	case ':':
		add_token(token_type::COLON);
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
		if (match('/')) // this is a line comment
		{
			consume_line_comment();
		}
		else if (match('*')) // this is a block comment
		{
			consume_block_comment();
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

	case '"':
		scan_string();
		break;

	default:
		if (validator::valid_number_literal_component(c))
		{
			scan_number_literal();
		}
		else if (validator::valid_identifier_component(c))
		{
			scan_identifier();
		}
		else
		{
			logging::logger::instance().error(line_, format("Unexpected character {}.", c));
		}

		break;
	}
}

void scanner::consume_block_comment()
{
	while (!is_end())
	{
		auto c = advance();
		if (c == '\n')line_++;
		else if (c == '*' && peek() == '/')
		{
			[[maybe_unused]]auto _ = advance(); // eat "/"
			return;
		}
		else if (c == '/' && peek() == '*') // nested block comment
		{
			[[maybe_unused]]auto _ = advance(); // eat "*"
			consume_block_comment();
		}
	}

	// not enough code to find next close sign */, so it's an error
	if (is_end())
	{
		logging::logger::instance().error(line_, "Unclosed block comment found.");
	}
}

void scanner::consume_line_comment()
{
	while (peek() != '\n' && !is_end())advance();
}

scanner::scanner(std::string src)
		: src_(std::move(src))
{
	for (const auto& p: keywords_to_type_)
	{
		type_to_keywords_[p.second] = p.first;
	}
}

std::string scanner::keyword_from_type(token_type t)
{
	return type_to_keywords_.at(t);
}

