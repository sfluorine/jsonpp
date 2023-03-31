#include <iostream>

#include "jsonval.h"

auto main() -> int {
    auto object = make_json_value<JsonObject>( JsonObject {
        { "number", make_json_value<JsonNumber>(3.14) },
        { "string", make_json_value<JsonString>("hello") },
        { "object", make_json_value<JsonObject>(
                JsonObject {
                { "a", make_json_value<JsonNumber>(8) },
                { "b", make_json_value<JsonNumber>(9) },
                })
        },
        { "array", make_json_value<JsonArray>(
                JsonArray {
                make_json_value<JsonNumber>(834),
                make_json_value<JsonString>("world"),
                })
        }
    });

    std::cout << object->serialize() << '\n';

    return 0;
}
