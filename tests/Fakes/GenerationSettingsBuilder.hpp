#pragma once

#include "Application/Generation/GenerationSettings.hpp"

#include <cstddef>

namespace sloptour::tests {

constexpr std::size_t TEST_MAX_STEPS = 5;
constexpr std::size_t TEST_QUIZ_QUESTIONS = 3;

[[nodiscard]] inline application::GenerationSettings test_settings(std::size_t quiz_questions = TEST_QUIZ_QUESTIONS) {
    return {.parallel_chapters = 2,
            .repair_attempts = 1,
            .default_level = "medium",
            .depths = {{.name = "light",
                        .min_chapters = 1,
                        .max_chapters = 1,
                        .max_steps = 1,
                        .deepen_steps = 1,
                        .quiz_questions = quiz_questions,
                        .guidance = "light"},
                       {.name = "medium",
                        .min_chapters = 1,
                        .max_chapters = 2,
                        .max_steps = TEST_MAX_STEPS,
                        .deepen_steps = 2,
                        .quiz_questions = quiz_questions,
                        .guidance = "medium"}},
            .details = {{.name = "medium", .guidance = "medium detail"}}};
}

}
