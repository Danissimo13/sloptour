#pragma once

#include "Application/Ports/TokenUsage.hpp"
#include "Domain/Tour/TourId.hpp"

#include <cstddef>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace sloptour::application {

struct RepositoryInspected {
    std::string repository;
    std::string scope;
};

struct OutlineStarted {
    std::string agent;
};

struct OutlineReady {
    std::string title;
    std::vector<std::string> chapters;
};

struct ChapterStarted {
    std::size_t index = 0;
};

struct ChapterReady {
    std::size_t index = 0;
    std::size_t steps = 0;
};

struct ChapterFailed {
    std::size_t index = 0;
    std::string reason;
};

struct AgentOutput {
    std::optional<std::size_t> chapter;
    std::string line;
};

struct TokensUsed {
    std::optional<std::size_t> chapter;
    TokenUsage usage;
};

struct QuizStarted {};

struct QuizReady {
    std::size_t questions = 0;
};

struct QuizFailed {
    std::string reason;
};

struct TourSaved {
    domain::TourId id;
};

using GenerationEvent =
    std::variant<RepositoryInspected, OutlineStarted, OutlineReady, ChapterStarted, ChapterReady, ChapterFailed,
                 AgentOutput, TokensUsed, QuizStarted, QuizReady, QuizFailed, TourSaved>;

}
