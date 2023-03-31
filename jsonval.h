#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <initializer_list>
#include <functional>

template <typename T>
class ValueAccessor {
public:
    virtual ~ValueAccessor() = default;

    virtual auto access(std::function<void(T&)>) -> void = 0;

protected:
    ValueAccessor() = default;
};

enum class JsonValueType {
    JsonBool,
    JsonNumber,
    JsonString,
    JsonObject,
    JsonArray,
    JsonNull,
};

class JsonValue {
public:
    virtual ~JsonValue() = default;

    virtual auto serialize() const -> std::string = 0;

    virtual auto get_type() const -> JsonValueType = 0;

protected:
    JsonValue() = default;
};

class JsonBool : public JsonValue, ValueAccessor<bool> {
public:
    JsonBool(bool boolean)
        : m_boolean(boolean) {}

    virtual auto serialize() const -> std::string override;

    virtual auto get_type() const -> JsonValueType override;

    virtual auto access(std::function<void(bool&)>) -> void override;

private:
    bool m_boolean;
};

class JsonNumber : public JsonValue, ValueAccessor<double> {
public:
    JsonNumber(double number)
        : m_number(number) {}

    virtual auto serialize() const -> std::string override;

    virtual auto get_type() const -> JsonValueType override;

    virtual auto access(std::function<void(double&)>) -> void override;

private:
    double m_number;
};

class JsonString : public JsonValue, ValueAccessor<std::string> {
public:
    JsonString(std::string string)
        : m_string(std::move(string)) {}

    JsonString(JsonString&& other)
        : m_string(std::move(other.m_string)) {}

    virtual auto serialize() const -> std::string override;

    virtual auto get_type() const -> JsonValueType override;

    virtual auto access(std::function<void(std::string&)>) -> void override;

private:
    std::string m_string;
};

using JsonObjectDict = std::unordered_map<std::string, std::shared_ptr<JsonValue>>;

class JsonObject : public JsonValue, ValueAccessor<JsonObjectDict> {
public:
    JsonObject(std::initializer_list<std::tuple<const char*, std::shared_ptr<JsonValue>>> list) {
        for (const auto& [key, value] : list)
            m_dict[key] = value;
    }

    JsonObject(JsonObject&& other) 
        : m_dict(std::move(other.m_dict)) {}

    virtual auto serialize() const -> std::string override;

    virtual auto get_type() const -> JsonValueType override;

    virtual auto access(std::function<void(JsonObjectDict&)>) -> void override;

private:
    JsonObjectDict m_dict;
};

using JsonArrayElements = std::vector<std::shared_ptr<JsonValue>>;

class JsonArray : public JsonValue, ValueAccessor<JsonArrayElements> {
public:
    JsonArray(std::initializer_list<std::shared_ptr<JsonValue>> list) {
        for (const auto& elem : list)
            m_array.push_back(elem);
    }

    JsonArray(JsonArray&& other)
        : m_array(std::move(other.m_array)) {}

    virtual auto serialize() const -> std::string override;

    virtual auto get_type() const -> JsonValueType override;

    virtual auto access(std::function<void(JsonArrayElements&)>) -> void override;

private:
    JsonArrayElements m_array;
};

class JsonNull : public JsonValue {
public:
    virtual auto serialize() const -> std::string override;

    virtual auto get_type() const -> JsonValueType override;
};

template <typename T, typename ... Args, typename = std::enable_if_t<std::is_base_of_v<JsonValue, T>>>
auto make_json_value(Args&& ... args) -> std::shared_ptr<T> {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T, typename = std::enable_if_t<std::is_base_of_v<JsonValue, T>>>
auto cast_json_value(std::shared_ptr<JsonValue> value) -> T* {
    return dynamic_cast<T*>(value.get());
}
