#include <iostream>
#include <fstream>
#include <sstream>

#include "parser.h"

auto main() -> int {
    std::ifstream file("dummy.json");
    std::stringstream ss;
    ss << file.rdbuf();

    const auto str = ss.str();

    auto json = std::get<1>(JsonParser::parse(str));

    // TODO: implement a more friendly and safe way to access values inside json object.
    // currently you gotta do the "not friendly and unsafe" way lmao :^)
    json.access([](JsonObjectDict& dict){
            auto player = cast_json_value<JsonObject>(dict["player"]);
            player->access([](JsonObjectDict& dict) {
                    dict["health"] = make_json_value<JsonNumber>(0);
                    dict["isDead"] = make_json_value<JsonBool>(true);
                    dict["friends"] = make_json_value<JsonArray>(JsonArray {
                            make_json_value<JsonString>("asep"),
                            make_json_value<JsonString>("dadang"),
                            make_json_value<JsonString>("jajang"),
                            });
                    dict["pos_x"] = make_json_value<JsonNull>();
                    dict["pos_y"] = make_json_value<JsonNull>();
                    });
            });

    // TODO: prettify json serializer
    std::cout << json.serialize() << '\n';

    return 0;
}
