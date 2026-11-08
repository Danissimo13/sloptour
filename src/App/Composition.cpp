#include "App/Composition.hpp"

#include "App/TerminalSettingsDocument.hpp"
#include "Infrastructure/Agents/ConfiguredAgentProvider.hpp"
#include "Infrastructure/Agents/JsonReplyDecoder.hpp"
#include "Infrastructure/Configuration/ConfigLoader.hpp"
#include "Infrastructure/Editor/ProcessEditor.hpp"
#include "Infrastructure/Git/GitRepositoryInspector.hpp"
#include "Infrastructure/Git/RepositoryRoot.hpp"
#include "Infrastructure/Platform/Platform.hpp"
#include "Infrastructure/Prompts/TemplatePromptLibrary.hpp"
#include "Infrastructure/Storage/GlobalStorage.hpp"
#include "Infrastructure/Storage/JsonPreferencesStore.hpp"
#include "Infrastructure/Storage/JsonProgressRepository.hpp"
#include "Infrastructure/Storage/JsonTourRepository.hpp"
#include "Infrastructure/Storage/MemoryProgressRepository.hpp"
#include "Infrastructure/Storage/MemoryTourRepository.hpp"
#include "Infrastructure/Storage/RepositorySourceFiles.hpp"
#include "Infrastructure/Storage/RoutedProgressRepository.hpp"
#include "Infrastructure/Storage/RoutedTourRepository.hpp"
#include "Infrastructure/Storage/StorageLayout.hpp"
#include "Infrastructure/Storage/StorageRouter.hpp"
#include "Infrastructure/Time/SystemClock.hpp"

#include <map>
#include <memory>
#include <vector>

namespace sloptour::app {

namespace {

constexpr std::chrono::milliseconds POLL_INTERVAL{50};

[[nodiscard]] std::vector<std::filesystem::path> config_overrides(const std::filesystem::path& root,
                                                                  const std::string& storage) {
    std::vector<std::filesystem::path> overrides;
    if (const auto directory = infrastructure::platform::user_config_directory()) {
        overrides.push_back(*directory / infrastructure::storage_layout::CONFIG);
    }
    overrides.push_back(root / storage / infrastructure::storage_layout::CONFIG);
    return overrides;
}

[[nodiscard]] std::shared_ptr<infrastructure::StorageRouter> storage_router(const std::filesystem::path& root,
                                                                            const std::filesystem::path& storage) {
    std::map<application::StorageMode, infrastructure::StorageBackend> backends;
    backends.emplace(
        application::StorageMode::Repository,
        infrastructure::StorageBackend{.tours = std::make_shared<infrastructure::JsonTourRepository>(storage),
                                       .progress = std::make_shared<infrastructure::JsonProgressRepository>(storage),
                                       .location = infrastructure::platform::display_path(storage)});
    if (const auto global = infrastructure::global_storage_directory(root)) {
        backends.emplace(application::StorageMode::Global,
                         infrastructure::StorageBackend{
                             .tours = std::make_shared<infrastructure::JsonTourRepository>(*global),
                             .progress = std::make_shared<infrastructure::JsonProgressRepository>(*global),
                             .location = infrastructure::platform::display_path(*global)});
    }
    backends.emplace(
        application::StorageMode::Memory,
        infrastructure::StorageBackend{.tours = std::make_shared<infrastructure::MemoryTourRepository>(),
                                       .progress = std::make_shared<infrastructure::MemoryProgressRepository>(),
                                       .location = {}});
    return std::make_shared<infrastructure::StorageRouter>(std::move(backends));
}

[[nodiscard]] std::shared_ptr<application::UserPreferences>
preferences_of(const infrastructure::AppConfig& config, const std::shared_ptr<infrastructure::StorageRouter>& router,
               const CompositionOverrides& overrides) {
    const auto directory = infrastructure::platform::user_config_directory().value_or(std::filesystem::path("."));
    auto store =
        std::make_shared<infrastructure::JsonPreferencesStore>(directory / infrastructure::storage_layout::PREFERENCES);
    auto initial = store->load(config.preferences);
    initial.language = overrides.language.value_or(initial.language);
    initial.storage = overrides.storage.value_or(initial.storage);
    return std::make_shared<application::UserPreferences>(application::UserPreferences::Dependencies{
        .store = store, .storage = router, .initial = initial, .persistent = true});
}

[[nodiscard]] terminal::TerminalServices services_of(const infrastructure::AppConfig& config,
                                                     const std::filesystem::path& root,
                                                     const std::filesystem::path& storage,
                                                     const CompositionOverrides& overrides) {
    const auto runner = std::make_shared<const infrastructure::ProcessRunner>(POLL_INTERVAL);
    const auto agents = std::make_shared<const infrastructure::ConfiguredAgentProvider>(
        infrastructure::ConfiguredAgentProvider::Dependencies{.profiles = config.agents,
                                                              .default_agent = config.default_agent,
                                                              .working_directory = root,
                                                              .runner = runner});
    const auto prompts = std::make_shared<const infrastructure::TemplatePromptLibrary>(
        storage / infrastructure::storage_layout::PROMPTS);
    const auto decoder = std::make_shared<const infrastructure::JsonReplyDecoder>();
    const auto router = storage_router(root, storage);
    const auto tours = std::make_shared<infrastructure::RoutedTourRepository>(router);
    const auto progress = std::make_shared<infrastructure::RoutedProgressRepository>(router);
    const auto inspector = std::make_shared<const infrastructure::GitRepositoryInspector>(root, runner, config.git);
    const auto quizzer = std::make_shared<const application::ComposeQuiz>(application::ComposeQuiz::Dependencies{
        .agents = agents, .prompts = prompts, .decoder = decoder, .settings = config.generation});
    return {
        .library = std::make_shared<const application::TourLibrary>(application::TourLibrary::Dependencies{
            .tours = tours,
            .progress = progress,
            .sources = std::make_shared<const infrastructure::RepositorySourceFiles>(root)}),
        .workspace = std::make_shared<const application::Workspace>(inspector, agents, config.generation),
        .generator = std::make_shared<const application::GenerateTour>(
            application::GenerateTour::Dependencies{.agents = agents,
                                                    .prompts = prompts,
                                                    .decoder = decoder,
                                                    .tours = tours,
                                                    .inspector = inspector,
                                                    .clock = std::make_shared<const infrastructure::SystemClock>(),
                                                    .quiz = quizzer,
                                                    .settings = config.generation}),
        .asker = std::make_shared<const application::AskAboutStep>(agents, prompts),
        .deepener = std::make_shared<const application::DeepenStep>(application::DeepenStep::Dependencies{
            .agents = agents, .prompts = prompts, .decoder = decoder, .settings = config.generation}),
        .quizzer = quizzer,
        .editor = std::make_shared<const infrastructure::ProcessEditor>(root, config.editor),
        .preferences = preferences_of(config, router, overrides),
    };
}

}

application::DecodeResult<Composition> compose(const std::filesystem::path& start,
                                               const CompositionOverrides& overrides) {
    const infrastructure::ProcessRunner runner(POLL_INTERVAL);
    auto root = infrastructure::repository_root(start, runner);
    auto defaults = infrastructure::load_config({});
    if (!defaults) {
        return std::unexpected(defaults.error());
    }
    auto config = infrastructure::load_config(config_overrides(root, defaults->storage_directory));
    if (!config) {
        return std::unexpected(config.error());
    }
    auto terminal = terminal_settings(config->terminal, config->git.recent_commits);
    if (!terminal) {
        return std::unexpected(terminal.error());
    }
    auto storage = root / config->storage_directory;
    auto services = services_of(*config, root, storage, overrides);
    return Composition{.root = std::move(root),
                       .storage = std::move(storage),
                       .services = std::move(services),
                       .terminal = std::move(*terminal)};
}

}
