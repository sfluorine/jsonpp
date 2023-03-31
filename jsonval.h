#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <initializer_list>

template <typename T>
class ValueGetter {
public:
    virtual ~ValueGetter() = default;

    virtual auto get_value() const -> T = 0;
    virtual auto get_value_ref() -> T& = 0;

protected:
    ValueGetter() = default;
};

class JsonValue {
public:
    virtual ~JsonValue() = default;

    virtual auto serialize() const -> std::string = 0;

protected:
    JsonValue() = default;
};

class JsonNumber : public JsonValue {
public:
    JsonNumber(double value)
        : m_value(value) {}

    virtual auto serialize() const -> std::string override;

private:
    double m_value;
};

class JsonString : public JsonValue {
public:
    JsonString(std::string string)
        : m_string(std::move(string)) {}

    JsonString(JsonString&& other)
        : m_string(std::move(other.m_string)) {}

    virtual auto serialize() const -> std::string override;

private:
    std::string m_string;
};

class JsonObject : public JsonValue {
public:
    JsonObject(std::initializer_list<std::tuple<const char*, std::shared_ptr<JsonValue>>> list) {
        for (const auto& [key, value] : list)
            m_dict[key] = value;
    }

    JsonObject(JsonObject&& other) 
        : m_dict(std::move(other.m_dict)) {}

    virtual auto serialize() const -> std::string override;

private:
    std::unordered_map<std::string, std::shared_ptr<JsonValue>> m_dict;
};

class JsonArray : public JsonValue {
public:
    JsonArray(std::initializer_list<std::shared_ptr<JsonValue>> list) {
        for (const auto& elem : list)
            m_array.push_back(elem);
    }

    JsonArray(JsonArray&& other)
        : m_array(std::move(other.m_array)) {}

    virtual auto serialize() const -> std::string override;

private:
    std::vector<std::shared_ptr<JsonValue>> m_array;
};

template <typename T, typename Args, typename = std::enable_if_t<std::is_base_of_v<JsonValue, T>>>
auto make_json_value(Args&& args) -> std::shared_ptr<T> {
    return std::make_shared<T>(std::forward<Args>(args));
}
