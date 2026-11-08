#pragma once

#include "Domain/Quiz/QuizQuestion.hpp"
#include "Domain/Tour/TourError.hpp"

#include <cstddef>
#include <map>
#include <utility>
#include <vector>

namespace sloptour::domain {

using QuizAnswers = std::map<std::size_t, std::size_t>;

class Quiz {
public:
    [[nodiscard]] static TourResult<Quiz> create(std::vector<QuizQuestion> questions);

    [[nodiscard]] const std::vector<QuizQuestion>& questions() const noexcept {
        return m_questions;
    }

    [[nodiscard]] std::size_t score(const QuizAnswers& answers) const noexcept;

private:
    explicit Quiz(std::vector<QuizQuestion> questions)
        : m_questions(std::move(questions)) {}

    std::vector<QuizQuestion> m_questions;
};

}
