#include "parser.h"
#include <cstdlib>

auto JsonParser::parse(std::string_view input) -> ErrorOr<JsonObject> {
    JsonParser parser(input);

    return parser.parse_json_object();
}

auto JsonParser::advance() -> void {
    if (m_current.type() == TokenType::EndOfFile)
        return;

    m_current = m_lexer.get_token();
}

auto JsonParser::expect(TokenType type) -> bool {
    return m_current.type() == type;
}

auto JsonParser::eat_token(TokenType type) -> void {
    if (not expect(type)) {
        std::string error;

        error.append("ERROR: expected: ");
        error.append(token_to_string(type));
        error.append(" but got: ");
        error.append(token_to_string(m_current.type()));

        m_error_stack.push_back(std::move(error));
    }

    advance();
}

auto JsonParser::parse_json_boolean() -> ErrorOr<JsonBool> {
    auto result = JsonBool(expect(TokenType::BooleanTrue) ? true : false);

    advance();

    return result;
}

auto JsonParser::parse_json_number() -> ErrorOr<JsonNumber> {
    if (not expect(TokenType::NumberLiteral)) {
        std::string error;

        error.append("ERROR: expected: number but got: ");
        error.append(token_to_string(m_current.type()));

        m_error_stack.push_back(std::move(error));

        return m_error_stack;
    }

    auto result = JsonNumber(std::strtod(m_current.lexeme().c_str(), nullptr));

    advance();

    return result;
}

auto JsonParser::parse_json_string() -> ErrorOr<JsonString> {
    if (not expect(TokenType::StringLiteral)) {
        std::string error;

        error.append("ERROR: expected: string but got: ");
        error.append(token_to_string(m_current.type()));

        m_error_stack.push_back(std::move(error));

        return m_error_stack;
    }

    auto result = JsonString(m_current.lexeme());

    advance();

    return result;
}

auto JsonParser::parse_json_object() -> ErrorOr<JsonObject> {
    eat_token(TokenType::OpenCurlyBrace);

    auto object = JsonObject{};

    if (expect(TokenType::CloseCurlyBrace)) {
        advance();
        return object;
    }

    while (true) {
        if (expect(TokenType::EndOfFile)) {
            std::string error;

            error.append("ERROR: unexpected end of file");

            m_error_stack.push_back(std::move(error));

            return m_error_stack;
        }


        const auto key = m_current.lexeme();

        eat_token(TokenType::StringLiteral);
        eat_token(TokenType::Colon);

        if (expect(TokenType::BooleanTrue)) {
            auto _true = TRY(parse_json_boolean());
            object.access([&key, &_true](JsonObjectDict& dict) {
                    dict[key] = make_json_value<JsonBool>(std::move(_true));
                    });
        } else if (expect(TokenType::BooleanFalse)) {
            auto _false = TRY(parse_json_boolean());
            object.access([&key, &_false](JsonObjectDict& dict) {
                    dict[key] = make_json_value<JsonBool>(std::move(_false));
                    });
        } else if (expect(TokenType::NumberLiteral)) {
            auto number = TRY(parse_json_number());
            object.access([&key, &number](JsonObjectDict& dict) {
                    dict[key] = make_json_value<JsonNumber>(std::move(number));
                    });
        } else if (expect(TokenType::StringLiteral)) {
            auto string_literal = TRY(parse_json_string());
            object.access([&key, &string_literal](JsonObjectDict& dict) {
                    dict[key] = make_json_value<JsonString>(std::move(string_literal));
                    });
        } else if (expect(TokenType::OpenCurlyBrace)) {
            auto _object = TRY(parse_json_object());
            object.access([&key, &_object](JsonObjectDict& dict) {
                    dict[key] = make_json_value<JsonObject>(std::move(_object));
                    });
        } else if (expect(TokenType::OpenBrace)) {
            auto array = TRY(parse_json_array());
            object.access([&key, &array](JsonObjectDict& dict) {
                    dict[key] = make_json_value<JsonArray>(std::move(array));
                    });
        } else if (expect(TokenType::Null)) {
            auto null = TRY(parse_json_null());
            object.access([&key, &null](JsonObjectDict& dict) {
                    dict[key] = make_json_value<JsonNull>(std::move(null));
                    });
        } else {
            std::string error;

            error.append("ERROR: unexpected token: ");
            error.append(token_to_string(m_current.type()));

            m_error_stack.push_back(std::move(error));

            return m_error_stack;
        }

        if (expect(TokenType::CloseCurlyBrace))
            break;
        
        eat_token(TokenType::Comma);
    }

    eat_token(TokenType::CloseCurlyBrace);

    if (m_error_stack.empty())
        return object;

    return m_error_stack;
}

auto JsonParser::parse_json_array() -> ErrorOr<JsonArray> {
    eat_token(TokenType::OpenBrace);

    auto array = JsonArray{};

    if (expect(TokenType::CloseBrace)) {
        advance();
        return array;
    }

    while (true) {
        if (expect(TokenType::EndOfFile)) {
            std::string error;

            error.append("ERROR: unexpected end of file");

            m_error_stack.push_back(std::move(error));

            return m_error_stack;
        }

        if (expect(TokenType::BooleanTrue)) {
            auto _true = TRY(parse_json_boolean());
            array.access([&_true](JsonArrayElements& elem) {
                    elem.push_back(make_json_value<JsonBool>(std::move(_true)));
                    });
        } else if (expect(TokenType::BooleanFalse)) {
            auto _false = TRY(parse_json_boolean());
            array.access([&_false](JsonArrayElements& elem) {
                    elem.push_back(make_json_value<JsonBool>(std::move(_false)));
                    });
        } else if (expect(TokenType::NumberLiteral)) {
            auto number = TRY(parse_json_number());
            array.access([&number](JsonArrayElements& elem) {
                    elem.push_back(make_json_value<JsonNumber>(std::move(number)));
                    });
        } else if (expect(TokenType::StringLiteral)) {
            auto string_literal = TRY(parse_json_string());
            array.access([&string_literal](JsonArrayElements& elem) {
                    elem.push_back(make_json_value<JsonString>(std::move(string_literal)));
                    });
        } else if (expect(TokenType::OpenCurlyBrace)) {
            auto _object = TRY(parse_json_object());
            array.access([&_object](JsonArrayElements& elem) {
                    elem.push_back(make_json_value<JsonObject>(std::move(_object)));
                    });
        } else if (expect(TokenType::OpenBrace)) {
            auto array = TRY(parse_json_array());
            array.access([&array](JsonArrayElements& elem) {
                    elem.push_back(make_json_value<JsonArray>(std::move(array)));
                    });
        } else if (expect(TokenType::Null)) {
            auto null = TRY(parse_json_null());
            array.access([&null](JsonArrayElements& elem) {
                    elem.push_back(make_json_value<JsonNull>(std::move(null)));
                    });
        } else {
            std::string error;

            error.append("ERROR: unexpected token: ");
            error.append(token_to_string(m_current.type()));

            m_error_stack.push_back(std::move(error));

            return m_error_stack;
        }

        if (expect(TokenType::CloseBrace))
            break;
        
        eat_token(TokenType::Comma);
    }

    eat_token(TokenType::CloseBrace);

    if (m_error_stack.empty())
        return array;

    return m_error_stack;

}

auto JsonParser::parse_json_null() -> ErrorOr<JsonNull> {
    if (not expect(TokenType::Null)) {
        std::string error;

        error.append("ERROR: expected: null but got: ");
        error.append(token_to_string(m_current.type()));

        m_error_stack.push_back(std::move(error));

        return m_error_stack;
    }

    auto result = JsonNull();

    advance();

    return result;
}
