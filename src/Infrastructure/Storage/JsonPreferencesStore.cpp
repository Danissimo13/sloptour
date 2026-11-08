#include "Infrastructure/Storage/JsonPreferencesStore.hpp"

#include "Core/Text/Text.hpp"
#include "Infrastructure/Json/JsonObjectBuilder.hpp"
#include "Infrastructure/Json/StorageModeNames.hpp"
#include "Infrastructure/Storage/StorageLayout.hpp"
#include "Infrastructure/Storage/TextFiles.hpp"

namespace sloptour::infrastructure {

namespace {

constexpr std::string_view LANGUAGE = "language";
constexpr std::string_view STORAGE = "storage";

}

application::Preferences JsonPreferencesStore::load(const application::Preferences& fallback) const {
    if (!is_file(m_file)) {
        return fallback;
    }
    auto text = read_text(m_file);
    auto document = text ? parsed_json(*text) : std::unexpected(application::DecodeError(text.error()));
    if (!document) {
        return fallback;
    }
    const auto language = optional_string(*document, LANGUAGE);
    return {.language = core::is_blank(language) ? fallback.language : language,
            .storage = storage_named(optional_string(*document, STORAGE)).value_or(fallback.storage)};
}

application::StorageResult<void> JsonPreferencesStore::save(const application::Preferences& preferences) {
    const auto document =
        JsonObjectBuilder().with(LANGUAGE, preferences.language).with(STORAGE, name_of(preferences.storage)).build();
    return write_text(m_file, document.dump(storage_layout::JSON_INDENT));
}

}
