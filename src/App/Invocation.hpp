#pragma once

#include "Domain/Tour/TourKind.hpp"

#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>
#include <variant>

namespace sloptour::app {

struct BrowseCommand {};

struct CreateCommand {
    domain::TourKind kind = domain::TourKind::Onboarding;
    std::optional<std::string> agent;
    std::string model;
    std::string depth;
    std::string detail;
    std::optional<std::string> since;
    std::optional<std::size_t> last;
    bool include_working_tree = true;
    std::string focus;
    std::string functionality;
    std::optional<std::string> name;
    bool plain = false;
};

struct PlayCommand {
    std::optional<std::string> tour;
};

struct ListCommand {};

struct AgentsCommand {};

struct EjectPromptsCommand {};

using Command =
    std::variant<BrowseCommand, CreateCommand, PlayCommand, ListCommand, AgentsCommand, EjectPromptsCommand>;

struct Invocation {
    std::filesystem::path repository;
    std::optional<std::string> language;
    std::optional<std::string> storage;
    Command command;
};

}
