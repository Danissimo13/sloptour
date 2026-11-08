#include "Application/Preferences/UserPreferences.hpp"
#include "Infrastructure/Storage/JsonPreferencesStore.hpp"
#include "Infrastructure/Storage/MemoryProgressRepository.hpp"
#include "Infrastructure/Storage/MemoryTourRepository.hpp"
#include "Infrastructure/Storage/RoutedTourRepository.hpp"
#include "Infrastructure/Storage/StorageRouter.hpp"
#include "tests/Fakes/TourBuilder.hpp"

#include <gtest/gtest.h>

#include <filesystem>

namespace sloptour::tests {

namespace {

[[nodiscard]] infrastructure::StorageBackend memory_backend(std::string location) {
    return {.tours = std::make_shared<infrastructure::MemoryTourRepository>(),
            .progress = std::make_shared<infrastructure::MemoryProgressRepository>(),
            .location = std::move(location)};
}

[[nodiscard]] std::shared_ptr<infrastructure::StorageRouter> router() {
    std::map<application::StorageMode, infrastructure::StorageBackend> backends;
    backends.emplace(application::StorageMode::Repository, memory_backend("repo"));
    backends.emplace(application::StorageMode::Memory, memory_backend(""));
    return std::make_shared<infrastructure::StorageRouter>(std::move(backends));
}

}

TEST(StorageTests, RouterKeepsToursApartPerMode) {
    const auto routes = router();
    infrastructure::RoutedTourRepository tours(routes);
    ASSERT_TRUE(tours.save(two_chapter_tour()).has_value());
    routes->use(application::StorageMode::Memory);
    EXPECT_TRUE(tours.list().value().empty());
    routes->use(application::StorageMode::Repository);
    EXPECT_EQ(tours.list().value().size(), 1U);
}

TEST(StorageTests, RouterIgnoresModesWithoutBackend) {
    const auto routes = router();
    routes->use(application::StorageMode::Global);
    EXPECT_EQ(routes->current(), application::StorageMode::Repository);
}

TEST(StorageTests, PreferencesSurviveARestart) {
    const auto file = std::filesystem::temp_directory_path() / "sloptour-preferences-test.json";
    std::filesystem::remove(file);
    const auto store = std::make_shared<infrastructure::JsonPreferencesStore>(file);
    const application::Preferences fallback{.language = "en", .storage = application::StorageMode::Repository};
    application::UserPreferences preferences(
        {.store = store, .storage = router(), .initial = fallback, .persistent = true});
    ASSERT_TRUE(preferences.choose_language("ru").has_value());
    ASSERT_TRUE(preferences.choose_storage(application::StorageMode::Memory).has_value());
    const auto restored = store->load(fallback);
    EXPECT_EQ(restored.language, "ru");
    EXPECT_EQ(restored.storage, application::StorageMode::Memory);
    std::filesystem::remove(file);
}

}
