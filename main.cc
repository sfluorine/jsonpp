#include <iostream>

#include "jsonval.h"

auto main() -> int {
    auto object = JsonObject {
        { "true", make_json_value<JsonBool>(true) },
        { "false", make_json_value<JsonBool>(false) },
        { "number", make_json_value<JsonNumber>(3.14) },
        { "string", make_json_value<JsonString>("hello") },
        { "object", make_json_value<JsonObject>(
                JsonObject {
                { "a", make_json_value<JsonNull>() },
                { "b", make_json_value<JsonNull>() },
                })
        },
        { "array", make_json_value<JsonArray>(
                JsonArray {
                make_json_value<JsonNumber>(834),
                make_json_value<JsonString>("world"),
                })
        },
        { "null", make_json_value<JsonNull>() },
    };

    object.access([](JsonObjectDict& x) {
        auto ptr = cast_json_value<JsonObject>(x["object"]);
        ptr->access([](JsonObjectDict& y) {
            y["a"] = make_json_value<JsonNumber>(34);
            y["b"] = make_json_value<JsonNumber>(35);
        });
    });

    std::cout << object.serialize() << '\n';

    return 0;
}
