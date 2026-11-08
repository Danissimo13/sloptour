#include "Infrastructure/Git/RepositoryRoot.hpp"

#include "Core/Text/Text.hpp"

#include <chrono>
#include <system_error>

namespace sloptour::infrastructure {

namespace {

constexpr std::chrono::seconds GIT_TIMEOUT{10};

}

std::filesystem::path repository_root(const std::filesystem::path& start, const ProcessRunner& runner) {
    std::error_code error;
    const auto absolute = std::filesystem::absolute(start, error);
    auto origin = error ? start : absolute;
    auto outcome = runner.run({.arguments = {"git", "rev-parse", "--show-toplevel"},
                               .input = {},
                               .working_directory = origin,
                               .timeout = GIT_TIMEOUT},
                              {.output = [](std::string_view /*line*/) {}, .errors = [](std::string_view /*line*/) {}},
                              std::stop_token{});
    if (!outcome || outcome->status != 0) {
        return origin;
    }
    const auto root = core::trim(outcome->output);
    return root.empty() ? origin : std::filesystem::path(root);
}

}
