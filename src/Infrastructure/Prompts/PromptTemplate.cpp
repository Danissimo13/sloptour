#include "Infrastructure/Prompts/PromptTemplate.hpp"

#include "Core/Text/Text.hpp"

namespace sloptour::infrastructure {

namespace {

constexpr std::string_view OPEN = "{{";
constexpr std::string_view CLOSE = "}}";

}

application::PromptResult<std::string> filled(std::string_view text, const application::PromptVariables& variables) {
    std::string result;
    result.reserve(text.size());
    std::size_t cursor = 0;
    for (auto open = text.find(OPEN); open != std::string_view::npos; open = text.find(OPEN, cursor)) {
        const auto close = text.find(CLOSE, open + OPEN.size());
        if (close == std::string_view::npos) {
            break;
        }
        const auto name = core::trim(text.substr(open + OPEN.size(), close - open - OPEN.size()));
        const auto value = variables.find(name);
        if (value == variables.end()) {
            return std::unexpected(application::PromptError("unknown prompt variable '" + std::string(name) + "'"));
        }
        result += text.substr(cursor, open - cursor);
        result += value->second;
        cursor = close + CLOSE.size();
    }
    result += text.substr(cursor);
    return result;
}

}
