#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace sloptour::core {

[[nodiscard]] std::string_view trim(std::string_view text) noexcept;
[[nodiscard]] std::vector<std::string_view> split_lines(std::string_view text);
[[nodiscard]] std::string join(const std::vector<std::string>& parts, std::string_view separator);
[[nodiscard]] std::string slugify(std::string_view text);
[[nodiscard]] bool is_blank(std::string_view text) noexcept;

}
