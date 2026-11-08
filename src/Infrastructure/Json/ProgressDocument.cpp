#include "Infrastructure/Json/ProgressDocument.hpp"

#include "Infrastructure/Json/DocumentKeys.hpp"
#include "Infrastructure/Json/JsonObjectBuilder.hpp"

namespace sloptour::infrastructure {

namespace {

[[nodiscard]] Json id_list(const std::set<domain::StepId>& ids) {
    Json list = Json::array();
    for (const auto& id : ids) {
        list.push_back(id.value());
    }
    return list;
}

[[nodiscard]] std::set<domain::StepId> id_set(const Json& document, std::string_view key) {
    std::set<domain::StepId> ids;
    for (const auto& raw : string_list(document, key)) {
        if (auto id = domain::StepId::create(raw)) {
            ids.insert(std::move(*id));
        }
    }
    return ids;
}

[[nodiscard]] Json exchanges_document(const std::map<domain::StepId, std::vector<domain::Exchange>>& exchanges) {
    JsonObjectBuilder builder;
    for (const auto& [step, list] : exchanges) {
        Json items = Json::array();
        for (const auto& exchange : list) {
            items.push_back(JsonObjectBuilder()
                                .with(keys::QUESTION, exchange.question)
                                .with(keys::ANSWER, exchange.answer)
                                .build());
        }
        builder.with(step.value(), std::move(items));
    }
    return builder.build();
}

[[nodiscard]] std::map<domain::StepId, std::vector<domain::Exchange>> exchanges_of(const Json& document) {
    std::map<domain::StepId, std::vector<domain::Exchange>> exchanges;
    const auto all = member(document, keys::EXCHANGES);
    if (!all || !all->get().is_object()) {
        return exchanges;
    }
    for (const auto& [key, items] : all->get().items()) {
        auto step = domain::StepId::create(key);
        if (!step || !items.is_array()) {
            continue;
        }
        auto& list = exchanges[*step];
        for (const auto& item : items) {
            list.push_back(
                {.question = optional_string(item, keys::QUESTION), .answer = optional_string(item, keys::ANSWER)});
        }
    }
    return exchanges;
}

[[nodiscard]] Json answers_document(const domain::QuizAnswers& answers) {
    Json list = Json::array();
    for (const auto& [question, option] : answers) {
        list.push_back(JsonObjectBuilder().with(keys::QUESTION, question).with(keys::ANSWER, option).build());
    }
    return list;
}

[[nodiscard]] domain::QuizAnswers answers_of(const Json& document) {
    domain::QuizAnswers answers;
    for (const auto& item : object_list(document, keys::QUIZ_ANSWERS)) {
        const auto question = optional_count(item, keys::QUESTION);
        const auto option = optional_count(item, keys::ANSWER);
        if (question && option) {
            answers.insert_or_assign(*question, *option);
        }
    }
    return answers;
}

}

Json progress_document(const domain::TourProgress& progress) {
    const auto& snapshot = progress.snapshot();
    JsonObjectBuilder builder;
    if (snapshot.current) {
        builder.with(keys::CURRENT, snapshot.current->value());
    }
    return builder.with(keys::VISITED, id_list(snapshot.visited))
        .with(keys::REVEALED, id_list(snapshot.revealed))
        .with(keys::EXCHANGES, exchanges_document(snapshot.exchanges))
        .with(keys::QUIZ_ANSWERS, answers_document(snapshot.quiz_answers))
        .build();
}

domain::TourProgress progress_from_document(domain::TourId tour, const Json& document) {
    domain::ProgressSnapshot snapshot{.current = std::nullopt,
                                      .visited = id_set(document, keys::VISITED),
                                      .revealed = id_set(document, keys::REVEALED),
                                      .exchanges = exchanges_of(document),
                                      .quiz_answers = answers_of(document)};
    if (auto current = domain::StepId::create(optional_string(document, keys::CURRENT))) {
        snapshot.current = std::move(*current);
    }
    return {std::move(tour), std::move(snapshot)};
}

}
