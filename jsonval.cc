#include "jsonval.h"

auto JsonNumber::serialize() const -> std::string {
    return std::to_string(m_value);
}

auto JsonString::serialize() const -> std::string {
    std::string result;

    result.push_back('"');
    result.append(m_string);
    result.push_back('"');

    return result;
}

auto JsonObject::serialize() const -> std::string {
    std::string result;

    result.push_back('{');

    for (const auto& [key, value] : m_dict) {
        result.push_back('"');
        result.append(key);
        result.push_back('"');

        result.push_back(':');
        result.append(value->serialize());
        result.push_back(',');
    }

    if (m_dict.size() > 0)
        result.pop_back();

    result.push_back('}');

    return result;
}

auto JsonArray::serialize() const -> std::string {
    std::string result;

    result.push_back('[');

    for (const auto& elem : m_array) {
        result.append(elem->serialize());
        result.push_back(',');
    }

    if (m_array.size() > 0)
        result.pop_back();

    result.push_back(']');

    return result;
}
