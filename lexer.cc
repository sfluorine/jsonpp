#include <cctype>

#include "lexer.h"

auto token_to_string(TokenType type) -> std::string {
    switch (type) {
    case TokenType::OpenCurlyBrace:
        return "{";
    case TokenType::CloseCurlyBrace:
        return "}";
    case TokenType::OpenBrace:
        return "[";
    case TokenType::CloseBrace:
        return "]";
    case TokenType::Colon:
        return ":";
    case TokenType::Comma:
        return ",";
    case TokenType::StringLiteral:
        return "string literal";
    case TokenType::NumberLiteral:
        return "number literal";
    case TokenType::BooleanTrue:
        return "true";
    case TokenType::BooleanFalse:
        return "false";
    case TokenType::Null:
        return "null";
    case TokenType::EndOfFile:
        return "end of file";
    case TokenType::Garbage:
        return "garbage";
    }
}

auto Token::to_string() const -> std::string {
    std::string result;

    result.append(token_to_string(m_type));

    if (m_lexeme.empty())
        return result;

    result.append(": ");
    result.append(m_lexeme);

    return result;
}

auto Token::type() const -> TokenType {
    return m_type;
}

auto Token::lexeme() const -> const std::string& {
    return m_lexeme;
}


auto JsonLexer::is_eof() const -> bool {
    return m_cursor >= m_input.length();
}

auto JsonLexer::get_token() -> Token {
    skip_whitespaces();

    if (is_eof())
        return Token(TokenType::EndOfFile);

    switch (current()) {
    case '{':
        advance();
        return Token(TokenType::OpenCurlyBrace);
    case '}':
        advance();
        return Token(TokenType::CloseCurlyBrace);
    case '[':
        advance();
        return Token(TokenType::OpenBrace);
    case ']':
        advance();
        return Token(TokenType::CloseBrace);
    case ':':
        advance();
        return Token(TokenType::Colon);
    case ',':
        advance();
        return Token(TokenType::Comma);
    case 't':
        return get_boolean();
    case 'f':
        return get_boolean();
    case 'n':
        return get_null();
    case '"':
        return get_string_literal();
    default:
        break;
    }

    if (std::isdigit(current()))
        return get_number_literal();

    std::string lexeme;

    do {
        lexeme.push_back(current());
        advance();
    } while (not is_eof() and not std::isspace(current()));

    return Token(TokenType::Garbage, std::move(lexeme));
}

auto JsonLexer::get_boolean() -> Token {
    std::string lexeme;

    do {
        lexeme.push_back(current());
        advance();
    } while (not is_eof() and std::isalpha(current()));

    if (lexeme == "true")
        return Token(TokenType::BooleanTrue);

    if (lexeme == "false")
        return Token(TokenType::BooleanFalse);

    return Token(TokenType::Garbage, std::move(lexeme));
}

auto JsonLexer::get_number_literal() -> Token {
    if (not std::isdigit(current())) {
        std::string lexeme;

        do {
            lexeme.push_back(current());
            advance();
        } while (not is_eof() and not std::isspace(current()));

        return Token(TokenType::Garbage, std::move(lexeme));
    }

    std::string number;

    do {
        number.push_back(current());
        advance();
    } while (not is_eof() and std::isdigit(current()));

    if (current() == '.') {
        number.push_back(current());
        advance();

        if (is_eof() or not std::isdigit(current())) {
            do {
                number.push_back(current());
                advance();
            } while (not is_eof() and not std::isspace(current()));

            return Token(TokenType::Garbage, std::move(number));
        }

        do {
            number.push_back(current());
            advance();
        } while (not is_eof() and std::isdigit(current()));
    }

    if (number[0] == '0')
        return Token(TokenType::Garbage, std::move(number));

    return Token(TokenType::NumberLiteral, std::move(number));
}

auto JsonLexer::get_string_literal() -> Token {
    if (current() != '"') {
        std::string lexeme;

        do {
            lexeme.push_back(current());
            advance();
        } while (not is_eof() and not std::isspace(current()));

        return Token(TokenType::Garbage, std::move(lexeme));
    }

    // skip the '"'
    advance();

    std::string content;

    do {
        content.push_back(current());
        advance();
    } while (not is_eof() and current() != '"');

    if (is_eof())
        return Token(TokenType::Garbage, std::move(content));

    // skip the '"'
    advance();

    return Token(TokenType::StringLiteral, std::move(content));
}

auto JsonLexer::get_null() -> Token {
    std::string lexeme;

    do {
        lexeme.push_back(current());
        advance();
    } while (not is_eof() and std::isalpha(current()));

    if (lexeme != "null")
        return Token(TokenType::Garbage, std::move(lexeme));

    return Token(TokenType::Null);
}

auto JsonLexer::current() const -> char {
    if (is_eof())
        return 0;

    return m_input[m_cursor];
}

auto JsonLexer::advance() -> void {
    if (is_eof())
        return;

    m_cursor++;
}

auto JsonLexer::skip_whitespaces() -> void {
    while (not is_eof() and std::isspace(current()))
        advance();
}
