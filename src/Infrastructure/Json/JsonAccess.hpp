#pragma once

#include "Application/Ports/PortErrors.hpp"

#include <nlohmann/json.hpp>

#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace sloptour::infrastructure {

using Json = nlohmann::json;
using JsonReference = std::reference_wrapper<const Json>;

[[nodiscard]] application::DecodeResult<Json> parsed_json(std::string_view text);
[[nodiscard]] std::optional<JsonReference> member(const Json& object, std::string_view key);
[[nodiscard]] application::DecodeResult<std::string> required_string(const Json& object, std::string_view key);
[[nodiscard]] std::string optional_string(const Json& object, std::string_view key);
[[nodiscard]] std::optional<std::size_t> optional_count(const Json& object, std::string_view key);
[[nodiscard]] application::DecodeResult<std::size_t> required_count(const Json& object, std::string_view key);
[[nodiscard]] std::vector<std::string> string_list(const Json& object, std::string_view key);
[[nodiscard]] std::vector<JsonReference> object_list(const Json& object, std::string_view key);

}
