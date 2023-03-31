#include "jsonval.h"

auto JsonBool::serialize() const -> std::string {
    return m_boolean ? "true" : "false";
}

auto JsonBool::get_type() const -> JsonValueType {
    return JsonValueType::JsonBool;
}

auto JsonBool::access(std::function<void(bool&)> func) -> void {
    func(m_boolean);
}

auto JsonNumber::serialize() const -> std::string {
    return std::to_string(m_number);
}

auto JsonNumber::get_type() const -> JsonValueType {
    return JsonValueType::JsonNumber;
}

auto JsonNumber::access(std::function<void(double&)> func) -> void {
    func(m_number);
}

auto JsonString::serialize() const -> std::string {
    std::string result;

    result.push_back('"');
    result.append(m_string);
    result.push_back('"');

    return result;
}

auto JsonString::get_type() const -> JsonValueType {
    return JsonValueType::JsonString;
}

auto JsonString::access(std::function<void(std::string&)> func) -> void {
    func(m_string);
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

auto JsonObject::get_type() const -> JsonValueType {
    return JsonValueType::JsonObject;
}

auto JsonObject::access(std::function<void(JsonObjectDict&)> func) -> void {
    func(m_dict);
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

auto JsonArray::get_type() const -> JsonValueType {
    return JsonValueType::JsonArray;
}

auto JsonArray::access(std::function<void(JsonArrayElements&)> func) -> void {
    func(m_array);
}

auto JsonNull::serialize() const -> std::string {
    return "null";
}

auto JsonNull::get_type() const -> JsonValueType {
    return JsonValueType::JsonNull;
}
