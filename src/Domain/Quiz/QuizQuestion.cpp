#include "Domain/Quiz/QuizQuestion.hpp"

#include "Core/Text/Text.hpp"

#include <algorithm>

namespace sloptour::domain {

namespace {

constexpr std::size_t MIN_OPTIONS = 2;

}

TourResult<QuizQuestion> QuizQuestion::create(std::string question, std::vector<std::string> options,
                                              std::size_t correct, std::string explanation) {
    if (core::is_blank(question)) {
        return std::unexpected(TourError("quiz question has no text"));
    }
    if (options.size() < MIN_OPTIONS || std::ranges::any_of(options, core::is_blank)) {
        return std::unexpected(TourError("quiz question needs at least two non-empty options"));
    }
    if (correct >= options.size()) {
        return std::unexpected(TourError("quiz answer points outside of the options"));
    }
    return QuizQuestion(std::move(question), std::move(options), correct, std::move(explanation));
}

}
