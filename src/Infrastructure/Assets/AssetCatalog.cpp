#include "Infrastructure/Assets/AssetCatalog.hpp"

#include "Infrastructure/Assets/EmbeddedAssets.hpp"

#include <algorithm>
#include <string>

namespace sloptour::infrastructure {

application::StorageResult<std::string_view> embedded_asset(std::string_view path) {
    const auto assets = embedded_assets();
    const auto found = std::ranges::find(assets, path, &EmbeddedAsset::path);
    if (found == assets.end()) {
        return std::unexpected(application::StorageError("missing embedded asset " + std::string(path)));
    }
    return found->content;
}

}
