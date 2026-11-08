#include "App/TerminalSettingsDocument.hpp"

#include "Infrastructure/Assets/AssetCatalog.hpp"
#include "Infrastructure/Assets/AssetPaths.hpp"
#include "Infrastructure/Assets/EmbeddedAssets.hpp"

namespace sloptour::app {

namespace {

constexpr std::string_view THEME = "theme";
constexpr std::string_view LANGUAGES = "languages";
constexpr std::string_view EXTENSIONS = "extensions";
constexpr std::string_view ALIASES = "aliases";
constexpr std::string_view LINE_COMMENT = "line_comment";
constexpr std::string_view KEYWORDS = "keywords";
constexpr std::string_view TERMINAL = "terminal";
constexpr std::string_view I18N_DIRECTORY = "i18n/";
constexpr std::string_view CODE = "code";
constexpr std::string_view LABEL = "label";
constexpr std::string_view AGENT_NAME = "agent_name";
constexpr std::string_view PLURAL_RULE = "plural_rule";
constexpr std::string_view TEXTS = "texts";

[[nodiscard]] terminal::ThemePalette palette_of(const infrastructure::Json& document) {
    terminal::ThemePalette palette;
    const auto theme = infrastructure::member(document, THEME);
    if (!theme || !theme->get().is_object()) {
        return palette;
    }
    for (const auto& [name, value] : theme->get().items()) {
        if (value.is_string()) {
            palette.emplace(name, value.get<std::string>());
        }
    }
    return palette;
}

[[nodiscard]] application::DecodeResult<std::vector<terminal::LanguageSyntax>> languages() {
    auto text = infrastructure::embedded_asset(infrastructure::asset_paths::SYNTAX);
    if (!text) {
        return std::unexpected(application::DecodeError(text.error()));
    }
    auto document = infrastructure::parsed_json(*text);
    if (!document) {
        return std::unexpected(document.error());
    }
    std::vector<terminal::LanguageSyntax> syntaxes;
    for (const auto& language : infrastructure::object_list(*document, LANGUAGES)) {
        syntaxes.push_back({.extensions = infrastructure::string_list(language, EXTENSIONS),
                            .aliases = infrastructure::string_list(language, ALIASES),
                            .line_comment = infrastructure::optional_string(language, LINE_COMMENT),
                            .keywords = infrastructure::string_list(language, KEYWORDS)});
    }
    return syntaxes;
}

[[nodiscard]] terminal::LanguagePack pack_of(const infrastructure::Json& document) {
    terminal::LanguagePack pack{.code = infrastructure::optional_string(document, CODE),
                                .label = infrastructure::optional_string(document, LABEL),
                                .agent_name = infrastructure::optional_string(document, AGENT_NAME),
                                .plural_rule = infrastructure::optional_string(document, PLURAL_RULE),
                                .texts = {}};
    const auto texts = infrastructure::member(document, TEXTS);
    if (!texts || !texts->get().is_object()) {
        return pack;
    }
    for (const auto& [key, value] : texts->get().items()) {
        if (value.is_string()) {
            pack.texts.emplace(key, value.get<std::string>());
        }
    }
    return pack;
}

[[nodiscard]] application::DecodeResult<std::vector<terminal::LanguagePack>> language_packs() {
    std::vector<terminal::LanguagePack> packs;
    for (const auto& asset : infrastructure::embedded_assets()) {
        if (!asset.path.starts_with(I18N_DIRECTORY)) {
            continue;
        }
        auto document = infrastructure::parsed_json(asset.content);
        if (!document) {
            return std::unexpected(document.error().located_at(asset.path));
        }
        packs.push_back(pack_of(*document));
    }
    if (packs.empty()) {
        return std::unexpected(application::DecodeError("no interface languages are embedded"));
    }
    return packs;
}

}

application::DecodeResult<terminal::TerminalSettings> terminal_settings(const infrastructure::Json& document,
                                                                        std::size_t recent_commits) {
    const auto outline = infrastructure::required_count(document, "outline_width");
    const auto context = infrastructure::required_count(document, "code_context_lines");
    const auto code = infrastructure::required_count(document, "code_max_lines");
    const auto activity = infrastructure::required_count(document, "activity_lines");
    const auto tab = infrastructure::required_count(document, "tab_width");
    const auto chat = infrastructure::required_count(document, "chat_height");
    auto syntaxes = languages();
    for (const auto& count : {outline, context, code, activity, tab, chat}) {
        if (!count) {
            return std::unexpected(count.error().located_at(TERMINAL));
        }
    }
    if (!syntaxes) {
        return std::unexpected(syntaxes.error());
    }
    auto packs = language_packs();
    if (!packs) {
        return std::unexpected(packs.error());
    }
    return terminal::TerminalSettings{.outline_width = *outline,
                                      .code_context_lines = *context,
                                      .code_max_lines = *code,
                                      .activity_lines = *activity,
                                      .tab_width = *tab,
                                      .chat_height = *chat,
                                      .recent_commits = recent_commits,
                                      .palette = palette_of(document),
                                      .languages = std::move(*syntaxes),
                                      .packs = std::move(*packs)};
}

}
