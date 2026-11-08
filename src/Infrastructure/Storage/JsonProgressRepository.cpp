#include "Infrastructure/Storage/JsonProgressRepository.hpp"

#include "Infrastructure/Json/ProgressDocument.hpp"
#include "Infrastructure/Storage/StorageLayout.hpp"
#include "Infrastructure/Storage/TextFiles.hpp"

namespace sloptour::infrastructure {

application::StorageResult<domain::TourProgress> JsonProgressRepository::load(const domain::TourId& id) const {
    const auto path = path_of(id);
    if (!is_file(path)) {
        return domain::TourProgress(id);
    }
    auto text = read_text(path);
    if (!text) {
        return std::unexpected(text.error());
    }
    auto document = parsed_json(*text);
    if (!document) {
        return domain::TourProgress(id);
    }
    return progress_from_document(id, *document);
}

application::StorageResult<void> JsonProgressRepository::save(const domain::TourProgress& progress) {
    return write_text(path_of(progress.tour()), progress_document(progress).dump(storage_layout::JSON_INDENT));
}

std::filesystem::path JsonProgressRepository::path_of(const domain::TourId& id) const {
    return m_directory / storage_layout::PROGRESS / (id.value() + std::string(storage_layout::DOCUMENT_EXTENSION));
}

}
