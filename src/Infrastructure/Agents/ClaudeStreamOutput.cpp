#include "Infrastructure/Agents/ClaudeStreamOutput.hpp"

#include "Core/Text/Text.hpp"
#include "Infrastructure/Json/JsonAccess.hpp"

#include <array>
#include <cstdint>

namespace sloptour::infrastructure {

namespace {

constexpr std::string_view TYPE = "type";
constexpr std::string_view ASSISTANT = "assistant";
constexpr std::string_view RESULT = "result";
constexpr std::string_view MESSAGE = "message";
constexpr std::string_view CONTENT = "content";
constexpr std::string_view TOOL_USE = "tool_use";
constexpr std::string_view NAME = "name";
constexpr std::string_view INPUT = "input";
constexpr std::string_view IS_ERROR = "is_error";
constexpr std::string_view USAGE = "usage";
constexpr std::string_view ID = "id";
constexpr std::string_view TOTAL_COST = "total_cost_usd";
constexpr std::array TOOL_ARGUMENTS{std::string_view("file_path"), std::string_view("path"),
                                    std::string_view("pattern"), std::string_view("command"),
                                    std::string_view("description")};

[[nodiscard]] std::string argument_of(const Json& input) {
    for (const auto key : TOOL_ARGUMENTS) {
        if (auto value = optional_string(input, key); !value.empty()) {
            return value;
        }
    }
    return {};
}

[[nodiscard]] std::int64_t tokens(const Json& usage, std::string_view key) {
    return static_cast<std::int64_t>(optional_count(usage, key).value_or(0));
}

[[nodiscard]] application::TokenUsage counted_usage(const Json& usage) {
    return {.input = tokens(usage, "input_tokens"),
            .output = tokens(usage, "output_tokens"),
            .cache_read = tokens(usage, "cache_read_input_tokens"),
            .cache_write = tokens(usage, "cache_creation_input_tokens"),
            .cost_usd = 0.0,
            .estimated = false};
}

[[nodiscard]] std::optional<UsageSample> usage_of(const Json& message) {
    const auto usage = member(message, USAGE);
    const auto id = optional_string(message, ID);
    if (!usage || id.empty()) {
        return std::nullopt;
    }
    return UsageSample{.key = id, .usage = counted_usage(*usage), .authoritative = false};
}

[[nodiscard]] std::optional<UsageSample> total_of(const Json& result) {
    const auto usage = member(result, USAGE);
    const auto cost = member(result, TOTAL_COST);
    if (!usage) {
        return std::nullopt;
    }
    auto total = counted_usage(*usage);
    if (cost && cost->get().is_number()) {
        total.cost_usd = cost->get().get<double>();
    }
    return UsageSample{.key = std::string(RESULT), .usage = total, .authoritative = true};
}

[[nodiscard]] std::optional<Json> event_of(std::string_view line) {
    auto event = parsed_json(core::trim(line));
    if (!event || !event->is_object()) {
        return std::nullopt;
    }
    return std::move(*event);
}

}

AgentLine ClaudeStreamOutput::parse(std::string_view line) const {
    const auto event = event_of(line);
    if (!event) {
        return {};
    }
    const auto type = optional_string(*event, TYPE);
    if (type == RESULT) {
        return {.activity = std::nullopt, .usage = total_of(*event)};
    }
    const auto message = member(*event, MESSAGE);
    if (type != ASSISTANT || !message) {
        return {};
    }
    return {.activity = activity(*message), .usage = usage_of(*message)};
}

std::optional<std::string> ClaudeStreamOutput::activity(const Json& message) const {
    for (const auto& item : object_list(message, CONTENT)) {
        if (optional_string(item, TYPE) == TOOL_USE) {
            const auto input = member(item, INPUT);
            const auto argument = input ? argument_of(*input) : std::string();
            return relative(optional_string(item, NAME) + " " + argument);
        }
    }
    return std::nullopt;
}

application::AgentResult<std::string> ClaudeStreamOutput::reply(std::string_view output) const {
    for (const auto line : core::split_lines(output)) {
        const auto event = event_of(line);
        if (!event || optional_string(*event, TYPE) != RESULT) {
            continue;
        }
        const auto failed = member(*event, IS_ERROR);
        auto text = optional_string(*event, RESULT);
        if (failed && failed->get().is_boolean() && failed->get().get<bool>()) {
            return std::unexpected(application::AgentError("claude reported an error: " + text));
        }
        return text;
    }
    return std::unexpected(application::AgentError("claude stream ended without a result"));
}

std::string ClaudeStreamOutput::relative(std::string text) const {
    for (auto found = text.find(m_prefix); found != std::string::npos; found = text.find(m_prefix, found)) {
        text.erase(found, m_prefix.size());
    }
    return text;
}

}
