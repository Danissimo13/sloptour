#include "Infrastructure/Json/QuizDocument.hpp"

#include "Infrastructure/Json/DocumentKeys.hpp"
#include "Infrastructure/Json/JsonObjectBuilder.hpp"

namespace sloptour::infrastructure {

namespace {

[[nodiscard]] application::DecodeResult<domain::QuizQuestion> question_from(const Json& document) {
    const auto answer = optional_count(document, keys::ANSWER);
    if (!answer) {
        return std::unexpected(application::DecodeError("quiz question has no answer index"));
    }
    auto question =
        domain::QuizQuestion::create(optional_string(document, keys::QUESTION), string_list(document, keys::OPTIONS),
                                     *answer, optional_string(document, keys::EXPLANATION));
    if (!question) {
        return std::unexpected(application::DecodeError(question.error()));
    }
    return std::move(*question);
}

}

Json quiz_document(const domain::Quiz& quiz) {
    Json questions = Json::array();
    for (const auto& question : quiz.questions()) {
        questions.push_back(JsonObjectBuilder()
                                .with(keys::QUESTION, question.question())
                                .with(keys::OPTIONS, question.options())
                                .with(keys::ANSWER, question.correct())
                                .with(keys::EXPLANATION, question.explanation())
                                .build());
    }
    return JsonObjectBuilder().with(keys::QUESTIONS, std::move(questions)).build();
}

application::DecodeResult<domain::Quiz> quiz_from_document(const Json& document) {
    std::vector<domain::QuizQuestion> questions;
    std::optional<application::DecodeError> first_failure;
    for (const auto& item : object_list(document, keys::QUESTIONS)) {
        auto question = question_from(item);
        if (question) {
            questions.push_back(std::move(*question));
        } else if (!first_failure) {
            first_failure = question.error();
        }
    }
    auto quiz = domain::Quiz::create(std::move(questions));
    if (!quiz) {
        const auto reason = first_failure ? first_failure->message() : quiz.error().message();
        return std::unexpected(application::DecodeError("no usable quiz question: " + reason));
    }
    return std::move(*quiz);
}

}
