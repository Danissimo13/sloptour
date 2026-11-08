#include "Infrastructure/Process/AttachedProcess.hpp"

#include "Infrastructure/Process/ExecutableLocator.hpp"

#include <reproc++/reproc.hpp>

namespace sloptour::infrastructure {

ProcessResult<int> run_attached(const std::vector<std::string>& arguments,
                                const std::filesystem::path& working_directory) {
    if (arguments.empty()) {
        return std::unexpected(ProcessError("empty command"));
    }
    reproc::process process;
    reproc::options options;
    const auto directory = working_directory.string();
    options.working_directory = directory.c_str();
    options.redirect.parent = true;
    if (const auto error = process.start(launchable(arguments), options)) {
        return std::unexpected(ProcessError("cannot start '" + arguments.front() + "': " + error.message()));
    }
    const auto [status, error] = process.wait(reproc::infinite);
    if (error) {
        return std::unexpected(ProcessError("cannot wait for '" + arguments.front() + "': " + error.message()));
    }
    return status;
}

}
