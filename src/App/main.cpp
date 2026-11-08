#include "App/CommandLine.hpp"
#include "App/CommandRunner.hpp"
#include "App/Composition.hpp"
#include "Infrastructure/Json/StorageModeNames.hpp"
#include "Infrastructure/Platform/Platform.hpp"

#include <print>
#include <variant>

int main(int argc, char** argv) {
    using namespace sloptour::app;
    sloptour::infrastructure::platform::ignore_broken_pipes();
    const auto parsed = parse_command_line(argc, argv);
    if (std::holds_alternative<ExitRequested>(parsed)) {
        return std::get<ExitRequested>(parsed).code;
    }
    const auto& invocation = std::get<Invocation>(parsed);
    auto composition =
        compose(invocation.repository,
                {.language = invocation.language, .storage = invocation.storage.and_then([](const std::string& name) {
                     return sloptour::infrastructure::storage_named(name);
                 })});
    if (!composition) {
        std::println(stderr, "sloptour: {}", composition.error().message());
        return 1;
    }
    return std::visit(CommandRunner(std::move(*composition)), invocation.command);
}
