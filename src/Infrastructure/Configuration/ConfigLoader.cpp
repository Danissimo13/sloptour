#include "Infrastructure/Configuration/ConfigLoader.hpp"

#include "Infrastructure/Assets/AssetCatalog.hpp"
#include "Infrastructure/Assets/AssetPaths.hpp"
#include "Infrastructure/Configuration/LevelsDocument.hpp"
#include "Infrastructure/Json/StorageModeNames.hpp"
#include "Infrastructure/Storage/TextFiles.hpp"

namespace sloptour::infrastructure {

namespace {

constexpr std::string_view AGENT = "agent";
constexpr std::string_view AGENTS = "agents";
constexpr std::string_view DESCRIPTION = "description";
constexpr std::string_view COMMAND = "command";
constexpr std::string_view TIMEOUT = "timeout_seconds";
constexpr std::string_view OUTPUT = "output";
constexpr std::string_view MODELS = "models";
constexpr std::string_view MODEL_ARGUMENTS = "model_arguments";
constexpr std::string_view GENERATION = "generation";
constexpr std::string_view GIT = "git";
constexpr std::string_view STORAGE = "storage";
constexpr std::string_view DIRECTORY = "directory";
constexpr std::string_view TERMINAL = "terminal";
constexpr std::string_view CONFIG = "config";
constexpr std::string_view EDITOR = "editor";
constexpr std::string_view PREFERENCES = "preferences";
constexpr std::string_view LANGUAGE = "language";
constexpr std::string_view STORAGE_MODE = "storage";
constexpr std::string_view FALLBACK = "fallback";
constexpr std::string_view ARGUMENTS = "arguments";
constexpr std::string_view DEFAULT_ARGUMENTS = "default";

template <typename... TResults>
[[nodiscard]] std::optional<application::DecodeError> first_error(const TResults&... results) {
    std::optional<application::DecodeError> error;
    const auto keep = [&error](const auto& result) {
        if (!error && !result) {
            error = result.error();
        }
    };
    (keep(results), ...);
    return error;
}

[[nodiscard]] application::DecodeResult<Json> section(const Json& document, std::string_view key) {
    const auto found = member(document, key);
    if (!found || !found->get().is_object()) {
        return std::unexpected(application::DecodeError("config section '" + std::string(key) + "' is missing"));
    }
    return found->get();
}

[[nodiscard]] application::DecodeResult<std::vector<AgentProfile>> agents_of(const Json& agents) {
    std::vector<AgentProfile> profiles;
    for (const auto& [name, settings] : agents.items()) {
        auto command = string_list(settings, COMMAND);
        if (command.empty()) {
            return std::unexpected(application::DecodeError("agent '" + name + "' has no command"));
        }
        profiles.push_back({.name = name,
                            .description = optional_string(settings, DESCRIPTION),
                            .command = std::move(command),
                            .output = optional_string(settings, OUTPUT),
                            .models = string_list(settings, MODELS),
                            .model_arguments = string_list(settings, MODEL_ARGUMENTS),
                            .timeout = std::chrono::seconds(optional_count(settings, TIMEOUT).value_or(0))});
    }
    return profiles;
}

[[nodiscard]] application::DecodeResult<application::GenerationSettings> generation_of(const Json& generation) {
    const auto parallel = required_count(generation, "parallel_chapters");
    const auto repairs = required_count(generation, "repair_attempts");
    auto fallback = required_string(generation, "default_level");
    auto depths = depth_levels(generation);
    auto details = detail_levels(generation);
    if (const auto error = first_error(parallel, repairs, fallback, depths, details)) {
        return std::unexpected(error->located_at(GENERATION));
    }
    return application::GenerationSettings{.parallel_chapters = *parallel,
                                           .repair_attempts = *repairs,
                                           .default_level = std::move(*fallback),
                                           .depths = std::move(*depths),
                                           .details = std::move(*details)};
}

[[nodiscard]] application::DecodeResult<GitSettings> git_of(const Json& git) {
    const auto commits = required_count(git, "recent_commits");
    const auto characters = required_count(git, "max_digest_characters");
    if (const auto error = first_error(commits, characters)) {
        return std::unexpected(error->located_at(GIT));
    }
    return GitSettings{.recent_commits = *commits, .max_digest_characters = *characters};
}

[[nodiscard]] application::DecodeResult<EditorSettings> editor_of(const Json& editor) {
    auto fallback = required_string(editor, FALLBACK);
    if (!fallback) {
        return std::unexpected(fallback.error().located_at(EDITOR));
    }
    EditorSettings settings{.fallback = std::move(*fallback), .default_arguments = {}, .arguments = {}};
    const auto patterns = member(editor, ARGUMENTS);
    if (!patterns || !patterns->get().is_object()) {
        return settings;
    }
    for (const auto& [name, arguments] : patterns->get().items()) {
        settings.arguments.emplace(name, string_list(patterns->get(), name));
    }
    settings.default_arguments = string_list(patterns->get(), DEFAULT_ARGUMENTS);
    return settings;
}

[[nodiscard]] application::DecodeResult<application::Preferences> preferences_of(const Json& preferences) {
    auto language = required_string(preferences, LANGUAGE);
    if (!language) {
        return std::unexpected(language.error().located_at(PREFERENCES));
    }
    const auto storage = storage_named(optional_string(preferences, STORAGE_MODE));
    if (!storage) {
        return std::unexpected(
            application::DecodeError("storage must be repository, global or memory").located_at(PREFERENCES));
    }
    return application::Preferences{.language = std::move(*language), .storage = *storage};
}

[[nodiscard]] application::DecodeResult<Json> merged_document(const std::vector<std::filesystem::path>& overrides) {
    auto defaults = embedded_asset(asset_paths::DEFAULT_CONFIG);
    if (!defaults) {
        return std::unexpected(application::DecodeError(defaults.error()));
    }
    auto document = parsed_json(*defaults);
    for (const auto& path : overrides) {
        if (!document || !is_file(path)) {
            continue;
        }
        auto text = read_text(path);
        auto patch = text ? parsed_json(*text) : std::unexpected(application::DecodeError(text.error()));
        if (!patch) {
            return std::unexpected(patch.error().located_at(path.string()));
        }
        document->merge_patch(*patch);
    }
    return document;
}

}

application::DecodeResult<AppConfig> load_config(const std::vector<std::filesystem::path>& overrides) {
    auto document = merged_document(overrides);
    if (!document) {
        return std::unexpected(document.error());
    }
    return config_from_document(*document);
}

application::DecodeResult<AppConfig> config_from_document(const Json& document) {
    auto agents = section(document, AGENTS).and_then(agents_of);
    auto generation = section(document, GENERATION).and_then(generation_of);
    auto git = section(document, GIT).and_then(git_of);
    auto storage =
        section(document, STORAGE).and_then([](const Json& json) { return required_string(json, DIRECTORY); });
    auto terminal = section(document, TERMINAL);
    auto editor = section(document, EDITOR).and_then(editor_of);
    auto preferences = section(document, PREFERENCES).and_then(preferences_of);
    auto agent = required_string(document, AGENT);
    if (const auto error = first_error(agents, generation, git, storage, terminal, agent, editor, preferences)) {
        return std::unexpected(error->located_at(CONFIG));
    }
    return AppConfig{.default_agent = std::move(*agent),
                     .agents = std::move(*agents),
                     .generation = *generation,
                     .git = *git,
                     .storage_directory = std::move(*storage),
                     .editor = std::move(*editor),
                     .preferences = std::move(*preferences),
                     .terminal = std::move(*terminal)};
}

}
