#pragma once

#include <string>
#include <string_view>

enum class TokenType {
    OpenCurlyBrace, /* { */
    CloseCurlyBrace,  /* } */
    OpenBrace, /* [ */
    CloseBrace, /* ] */

    Colon, /* : */
    Comma, /* , */

    StringLiteral,
    NumberLiteral,

    BooleanTrue, /* true */
    BooleanFalse, /* false */

    Null, /* null */

    EndOfFile,
    Garbage,
};

auto token_to_string(TokenType) -> std::string_view;

class Token {
public:
    Token() = default;

    Token(TokenType type)
        : m_type(type) {}

    Token(TokenType type, std::string lexeme)
        : m_type(type), m_lexeme(std::move(lexeme)) {}

    Token(Token&& other)
        : m_type(other.m_type), m_lexeme(std::move(other.m_lexeme)) 
    {
        other.m_type = TokenType::Garbage;
    }

    auto operator=(const Token& other) -> Token& = delete;

    auto operator=(Token&& other) -> Token& {
        m_type = other.m_type;
        m_lexeme = std::move(other.m_lexeme);

        other.m_type = TokenType::Garbage;
        return *this;
    }

    auto to_string() const -> std::string;

    auto type() const -> TokenType;

    auto lexeme() const -> const std::string&;

private:
    TokenType m_type{TokenType::Garbage};
    std::string m_lexeme;
};

class JsonLexer {
public:
    JsonLexer(std::string_view input)
        : m_input(input) {}

    auto is_eof() const -> bool;

    auto get_token() -> Token;

private:
    auto get_boolean() -> Token;

    auto get_number_literal() -> Token;

    auto get_string_literal() -> Token;

    auto get_null() -> Token;

    auto current() const -> char;

    auto advance() -> void;

    auto skip_whitespaces() -> void;

private:
    std::size_t m_cursor{0};
    std::string_view m_input;
};
