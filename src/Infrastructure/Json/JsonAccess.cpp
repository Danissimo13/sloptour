#include "Infrastructure/Json/JsonAccess.hpp"

namespace sloptour::infrastructure {

namespace {

[[nodiscard]] application::DecodeError missing(std::string_view key, std::string_view kind) {
    return application::DecodeError("field '" + std::string(key) + "' must be " + std::string(kind));
}

}

application::DecodeResult<Json> parsed_json(std::string_view text) {
    auto json = Json::parse(text, nullptr, false);
    if (json.is_discarded()) {
        return std::unexpected(application::DecodeError("text is not valid JSON"));
    }
    return json;
}

std::optional<JsonReference> member(const Json& object, std::string_view key) {
    if (!object.is_object()) {
        return std::nullopt;
    }
    const auto found = object.find(key);
    if (found == object.end()) {
        return std::nullopt;
    }
    return std::cref(*found);
}

application::DecodeResult<std::string> required_string(const Json& object, std::string_view key) {
    const auto value = member(object, key);
    if (!value || !value->get().is_string()) {
        return std::unexpected(missing(key, "a string"));
    }
    return value->get().get<std::string>();
}

std::string optional_string(const Json& object, std::string_view key) {
    const auto value = member(object, key);
    if (!value || !value->get().is_string()) {
        return {};
    }
    return value->get().get<std::string>();
}

std::optional<std::size_t> optional_count(const Json& object, std::string_view key) {
    const auto value = member(object, key);
    if (!value || !value->get().is_number_integer() || value->get().get<long long>() < 0) {
        return std::nullopt;
    }
    return value->get().get<std::size_t>();
}

application::DecodeResult<std::size_t> required_count(const Json& object, std::string_view key) {
    const auto count = optional_count(object, key);
    if (!count) {
        return std::unexpected(missing(key, "a non-negative integer"));
    }
    return *count;
}

std::vector<std::string> string_list(const Json& object, std::string_view key) {
    std::vector<std::string> strings;
    const auto value = member(object, key);
    if (!value || !value->get().is_array()) {
        return strings;
    }
    for (const auto& item : value->get()) {
        if (item.is_string()) {
            strings.push_back(item.get<std::string>());
        }
    }
    return strings;
}

std::vector<JsonReference> object_list(const Json& object, std::string_view key) {
    std::vector<JsonReference> objects;
    const auto value = member(object, key);
    if (!value || !value->get().is_array()) {
        return objects;
    }
    for (const auto& item : value->get()) {
        if (item.is_object()) {
            objects.emplace_back(item);
        }
    }
    return objects;
}

}
