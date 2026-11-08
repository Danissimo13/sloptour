#include "Infrastructure/Json/TourDocument.hpp"

#include "Infrastructure/Json/DocumentKeys.hpp"
#include "Infrastructure/Json/JsonObjectBuilder.hpp"
#include "Infrastructure/Json/QuizDocument.hpp"
#include "Infrastructure/Json/StepDocument.hpp"
#include "Infrastructure/Json/StepListDocument.hpp"
#include "Infrastructure/Json/TourKindNames.hpp"

namespace sloptour::infrastructure {

namespace {

constexpr int FORMAT_VERSION = 1;

[[nodiscard]] Json chapter_document(const domain::Chapter& chapter) {
    Json steps = Json::array();
    for (const auto& step : chapter.steps()) {
        steps.push_back(step_document(step));
    }
    return JsonObjectBuilder()
        .with(keys::TITLE, chapter.title())
        .with(keys::SUMMARY, chapter.summary())
        .with(keys::STEPS, std::move(steps))
        .build();
}

[[nodiscard]] application::DecodeResult<domain::Chapter> chapter_from_document(const Json& document) {
    const auto steps = member(document, keys::STEPS);
    if (!steps) {
        return std::unexpected(application::DecodeError("chapter has no steps"));
    }
    auto decoded = steps_from_document(*steps);
    if (!decoded) {
        return std::unexpected(decoded.error());
    }
    auto chapter = domain::Chapter::create(optional_string(document, keys::TITLE),
                                           optional_string(document, keys::SUMMARY), std::move(*decoded));
    if (!chapter) {
        return std::unexpected(application::DecodeError(chapter.error()));
    }
    return std::move(*chapter);
}

[[nodiscard]] application::DecodeResult<std::vector<domain::Chapter>> chapters_of(const Json& document) {
    std::vector<domain::Chapter> chapters;
    for (const auto& item : object_list(document, keys::CHAPTERS)) {
        auto chapter = chapter_from_document(item);
        if (!chapter) {
            return std::unexpected(chapter.error());
        }
        chapters.push_back(std::move(*chapter));
    }
    return chapters;
}

[[nodiscard]] domain::TourMetadata metadata_of(const Json& document) {
    return {.title = optional_string(document, keys::TITLE),
            .summary = optional_string(document, keys::SUMMARY),
            .kind = kind_named(optional_string(document, keys::KIND)).value_or(domain::TourKind::Onboarding),
            .repository = optional_string(document, keys::REPOSITORY),
            .scope = optional_string(document, keys::SCOPE),
            .agent = optional_string(document, keys::AGENT),
            .model = optional_string(document, keys::MODEL),
            .depth = optional_string(document, keys::DEPTH),
            .detail = optional_string(document, keys::DETAIL),
            .language = optional_string(document, keys::LANGUAGE),
            .created_at = optional_string(document, keys::CREATED_AT)};
}

}

Json tour_document(const domain::Tour& tour) {
    Json chapters = Json::array();
    for (const auto& chapter : tour.chapters()) {
        chapters.push_back(chapter_document(chapter));
    }
    const auto& metadata = tour.metadata();
    JsonObjectBuilder builder;
    if (const auto& quiz = tour.quiz()) {
        builder.with(keys::QUIZ, quiz_document(*quiz));
    }
    return builder.with(keys::FORMAT, FORMAT_VERSION)
        .with(keys::ID, tour.id().value())
        .with(keys::TITLE, metadata.title)
        .with(keys::SUMMARY, metadata.summary)
        .with(keys::KIND, name_of(metadata.kind))
        .with(keys::REPOSITORY, metadata.repository)
        .with(keys::SCOPE, metadata.scope)
        .with(keys::AGENT, metadata.agent)
        .with(keys::MODEL, metadata.model)
        .with(keys::DEPTH, metadata.depth)
        .with(keys::DETAIL, metadata.detail)
        .with(keys::LANGUAGE, metadata.language)
        .with(keys::CREATED_AT, metadata.created_at)
        .with(keys::CHAPTERS, std::move(chapters))
        .build();
}

application::DecodeResult<domain::Tour> tour_from_document(const Json& document) {
    auto id = domain::TourId::create(optional_string(document, keys::ID));
    if (!id) {
        return std::unexpected(application::DecodeError(id.error()));
    }
    auto chapters = chapters_of(document);
    if (!chapters) {
        return std::unexpected(chapters.error());
    }
    auto tour = domain::Tour::create(std::move(*id), metadata_of(document), std::move(*chapters));
    if (!tour) {
        return std::unexpected(application::DecodeError(tour.error()));
    }
    if (const auto quiz = member(document, keys::QUIZ)) {
        if (auto decoded = quiz_from_document(*quiz)) {
            tour->attach_quiz(std::move(*decoded));
        }
    }
    return std::move(*tour);
}

}
