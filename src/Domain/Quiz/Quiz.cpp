#include "Domain/Quiz/Quiz.hpp"

namespace sloptour::domain {

TourResult<Quiz> Quiz::create(std::vector<QuizQuestion> questions) {
    if (questions.empty()) {
        return std::unexpected(TourError("quiz has no questions"));
    }
    return Quiz(std::move(questions));
}

std::size_t Quiz::score(const QuizAnswers& answers) const noexcept {
    std::size_t correct = 0;
    for (const auto& [question, option] : answers) {
        if (question < m_questions.size() && m_questions.at(question).is_correct(option)) {
            ++correct;
        }
    }
    return correct;
}

}
