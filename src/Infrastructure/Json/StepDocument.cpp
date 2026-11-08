#include "Infrastructure/Json/StepDocument.hpp"

#include "Core/Text/Text.hpp"
#include "Infrastructure/Json/DocumentKeys.hpp"
#include "Infrastructure/Json/JsonObjectBuilder.hpp"

namespace sloptour::infrastructure {

namespace {

[[nodiscard]] Json location_document(const domain::CodeLocation& location) {
    JsonObjectBuilder builder;
    builder.with(keys::PATH, location.path()).with(keys::NOTE, location.note());
    if (location.lines()) {
        builder.with(keys::START_LINE, location.lines()->first()).with(keys::END_LINE, location.lines()->last());
    }
    return builder.build();
}

[[nodiscard]] std::optional<domain::LineRange> lines_of(const Json& document) {
    const auto first = optional_count(document, keys::START_LINE);
    if (!first) {
        return std::nullopt;
    }
    const auto last = optional_count(document, keys::END_LINE).value_or(*first);
    auto range = domain::LineRange::create(*first, std::max(*first, last));
    return range ? std::optional(*range) : std::nullopt;
}

[[nodiscard]] std::vector<domain::CodeLocation> locations_of(const Json& document) {
    std::vector<domain::CodeLocation> locations;
    for (const auto& item : object_list(document, keys::LOCATIONS)) {
        auto location = domain::CodeLocation::create(optional_string(item, keys::PATH), lines_of(item),
                                                     optional_string(item, keys::NOTE));
        if (location) {
            locations.push_back(std::move(*location));
        }
    }
    return locations;
}

[[nodiscard]] std::optional<domain::Checkpoint> checkpoint_of(const Json& document) {
    const auto checkpoint = member(document, keys::CHECKPOINT);
    if (!checkpoint || !checkpoint->get().is_object()) {
        return std::nullopt;
    }
    return domain::Checkpoint{.question = optional_string(*checkpoint, keys::QUESTION),
                              .answer = optional_string(*checkpoint, keys::ANSWER)};
}

[[nodiscard]] application::DecodeResult<domain::StepId> id_of(const Json& document, const std::string& title) {
    auto raw = optional_string(document, keys::ID);
    auto id = domain::StepId::create(core::is_blank(raw) ? core::slugify(title) : raw);
    if (!id) {
        return std::unexpected(application::DecodeError(id.error()));
    }
    return std::move(*id);
}

}

Json step_document(const domain::Step& step) {
    Json locations = Json::array();
    for (const auto& location : step.locations()) {
        locations.push_back(location_document(location));
    }
    JsonObjectBuilder builder;
    builder.with(keys::ID, step.id().value())
        .with(keys::TITLE, step.title())
        .with(keys::BODY, step.body())
        .with(keys::LOCATIONS, std::move(locations))
        .with(keys::COMMANDS, step.commands());
    if (const auto& checkpoint = step.checkpoint()) {
        builder.with(keys::CHECKPOINT, JsonObjectBuilder()
                                           .with(keys::QUESTION, checkpoint->question)
                                           .with(keys::ANSWER, checkpoint->answer)
                                           .build());
    }
    return builder.build();
}

application::DecodeResult<domain::Step> step_from_document(const Json& document) {
    auto title = required_string(document, keys::TITLE);
    if (!title) {
        return std::unexpected(title.error());
    }
    auto id = id_of(document, *title);
    if (!id) {
        return std::unexpected(id.error());
    }
    auto step = domain::Step::create({.id = std::move(*id),
                                      .title = std::move(*title),
                                      .body = optional_string(document, keys::BODY),
                                      .locations = locations_of(document),
                                      .commands = string_list(document, keys::COMMANDS),
                                      .checkpoint = checkpoint_of(document)});
    if (!step) {
        return std::unexpected(application::DecodeError(step.error()));
    }
    return std::move(*step);
}

}
