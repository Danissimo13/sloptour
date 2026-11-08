#pragma once

#include "Application/Ports/IProgressRepository.hpp"
#include "Application/Ports/ITourRepository.hpp"

#include <memory>
#include <string>

namespace sloptour::infrastructure {

struct StorageBackend {
    std::shared_ptr<application::ITourRepository> tours;
    std::shared_ptr<application::IProgressRepository> progress;
    std::string location;
};

}
