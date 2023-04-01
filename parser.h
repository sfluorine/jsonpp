#pragma once

#include <vector>
#include <variant>

#include "lexer.h"
#include "jsonval.h"

using ErrorStack = std::vector<std::string>;

template <typename T>
using ErrorOr = std::variant<ErrorStack, T>;

#define TRY(...)\
    ({\
     auto res = __VA_ARGS__;\
     if (std::holds_alternative<ErrorStack>(res))\
        return std::get<0>(res);\
     std::move(std::get<1>(res));\
     })\

class JsonParser {
public:
    JsonParser(std::string_view input)
        : m_lexer(input), m_current(m_lexer.get_token()) {}

    static auto parse(std::string_view) -> ErrorOr<JsonObject>;

private:
    auto advance() -> void;

    auto expect(TokenType type) -> bool;

    auto eat_token(TokenType type) -> void;

    auto parse_json_boolean() -> ErrorOr<JsonBool>;

    auto parse_json_number() -> ErrorOr<JsonNumber>;

    auto parse_json_string() -> ErrorOr<JsonString>;

    auto parse_json_object() -> ErrorOr<JsonObject>;

    auto parse_json_array() -> ErrorOr<JsonArray>;

    auto parse_json_null() -> ErrorOr<JsonNull>;

private:
    JsonLexer m_lexer;
    Token m_current;
    ErrorStack m_error_stack;
};
