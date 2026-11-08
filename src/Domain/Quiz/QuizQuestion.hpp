#pragma once

#include "Domain/Tour/TourError.hpp"

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace sloptour::domain {

class QuizQuestion {
public:
    [[nodiscard]] static TourResult<QuizQuestion> create(std::string question, std::vector<std::string> options,
                                                         std::size_t correct, std::string explanation);

    [[nodiscard]] const std::string& question() const noexcept {
        return m_question;
    }

    [[nodiscard]] const std::vector<std::string>& options() const noexcept {
        return m_options;
    }

    [[nodiscard]] std::size_t correct() const noexcept {
        return m_correct;
    }

    [[nodiscard]] const std::string& explanation() const noexcept {
        return m_explanation;
    }

    [[nodiscard]] bool is_correct(std::size_t option) const noexcept {
        return option == m_correct;
    }

private:
    QuizQuestion(std::string question, std::vector<std::string> options, std::size_t correct, std::string explanation)
        : m_question(std::move(question))
        , m_options(std::move(options))
        , m_correct(correct)
        , m_explanation(std::move(explanation)) {}

    std::string m_question;
    std::vector<std::string> m_options;
    std::size_t m_correct;
    std::string m_explanation;
};

}
