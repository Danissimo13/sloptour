#include "Application/Exploration/StepBrief.hpp"

#include <format>

namespace sloptour::application {

namespace {

constexpr std::string_view NO_HISTORY = "(no earlier questions)";
constexpr std::size_t DIGEST_BODY_LENGTH = 280;

[[nodiscard]] std::string located(const domain::CodeLocation& location) {
    if (!location.lines()) {
        return location.path();
    }
    return std::format("{}:{}-{}", location.path(), location.lines()->first(), location.lines()->last());
}

}

std::string brief_of(const domain::Step& step) {
    auto brief = std::format("Step [{}] {}\n\n{}\n", step.id().value(), step.title(), step.body());
    if (!step.locations().empty()) {
        brief += "\nCode locations:\n";
        for (const auto& location : step.locations()) {
            brief += std::format("- {} {}\n", located(location), location.note());
        }
    }
    if (!step.commands().empty()) {
        brief += "\nCommands:\n";
        for (const auto& command : step.commands()) {
            brief += std::format("- {}\n", command);
        }
    }
    return brief;
}

std::string digest_of(const domain::Tour& tour) {
    std::string digest;
    std::size_t number = 1;
    for (const auto& chapter : tour.chapters()) {
        digest += std::format("Chapter {}: {}\n", number++, chapter.title());
        for (const auto& step : chapter.steps()) {
            const auto body = std::string_view(step.body()).substr(0, DIGEST_BODY_LENGTH);
            digest += std::format("- [{}] {}: {}\n", step.id().value(), step.title(), body);
        }
    }
    return digest;
}

std::string history_of(std::span<const domain::Exchange> exchanges) {
    if (exchanges.empty()) {
        return std::string(NO_HISTORY);
    }
    std::string history;
    for (const auto& exchange : exchanges) {
        history += std::format("Q: {}\nA: {}\n\n", exchange.question, exchange.answer);
    }
    return history;
}

}
