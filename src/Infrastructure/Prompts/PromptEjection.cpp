#include "Infrastructure/Prompts/PromptEjection.hpp"

#include "Infrastructure/Assets/AssetPaths.hpp"
#include "Infrastructure/Assets/EmbeddedAssets.hpp"
#include "Infrastructure/Storage/TextFiles.hpp"

namespace sloptour::infrastructure {

application::StorageResult<std::size_t> eject_prompts(const std::filesystem::path& directory) {
    std::size_t written = 0;
    for (const auto& asset : embedded_assets()) {
        if (!asset.path.starts_with(asset_paths::PROMPTS_DIRECTORY)) {
            continue;
        }
        const auto target = directory / asset.path.substr(asset_paths::PROMPTS_DIRECTORY.size());
        if (is_file(target)) {
            continue;
        }
        if (auto saved = write_text(target, asset.content); !saved) {
            return std::unexpected(saved.error());
        }
        ++written;
    }
    return written;
}

}
