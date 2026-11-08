#include "Infrastructure/Storage/JsonTourRepository.hpp"

#include "Infrastructure/Json/TourDocument.hpp"
#include "Infrastructure/Storage/StorageLayout.hpp"
#include "Infrastructure/Storage/TextFiles.hpp"

#include <algorithm>
#include <system_error>

namespace sloptour::infrastructure {

application::StorageResult<void> JsonTourRepository::save(const domain::Tour& tour) {
    return write_text(path_of(tour.id()), tour_document(tour).dump(storage_layout::JSON_INDENT));
}

application::StorageResult<domain::Tour> JsonTourRepository::load(const domain::TourId& id) const {
    const auto path = path_of(id);
    auto text = read_text(path);
    if (!text) {
        return std::unexpected(text.error());
    }
    auto document = parsed_json(*text);
    auto tour = document ? tour_from_document(*document) : std::unexpected(document.error());
    if (!tour) {
        return std::unexpected(application::StorageError(tour.error()).located_at(path.string()));
    }
    return std::move(*tour);
}

application::StorageResult<std::vector<domain::TourId>> JsonTourRepository::list() const {
    std::vector<domain::TourId> ids;
    std::error_code error;
    const auto directory = m_directory / storage_layout::TOURS;
    for (const auto& entry : std::filesystem::directory_iterator(directory, error)) {
        if (entry.path().extension() != storage_layout::DOCUMENT_EXTENSION) {
            continue;
        }
        if (auto id = domain::TourId::create(entry.path().stem().string())) {
            ids.push_back(std::move(*id));
        }
    }
    std::ranges::sort(ids);
    return ids;
}

std::filesystem::path JsonTourRepository::path_of(const domain::TourId& id) const {
    return m_directory / storage_layout::TOURS / (id.value() + std::string(storage_layout::DOCUMENT_EXTENSION));
}

}
