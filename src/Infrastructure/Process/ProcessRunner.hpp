#pragma once

#include "Infrastructure/Process/ProcessCommand.hpp"
#include "Infrastructure/Process/ProcessError.hpp"
#include "Infrastructure/Process/ProcessListeners.hpp"
#include "Infrastructure/Process/ProcessOutcome.hpp"

#include <chrono>
#include <filesystem>
#include <stop_token>
#include <string>
#include <vector>

namespace sloptour::infrastructure {

class ProcessRunner {
public:
    explicit ProcessRunner(std::chrono::milliseconds poll_interval)
        : m_poll_interval(poll_interval) {}

    [[nodiscard]] ProcessResult<ProcessOutcome> run(const ProcessCommand& command, const ProcessListeners& listeners,
                                                    const std::stop_token& stop) const;

private:
    std::chrono::milliseconds m_poll_interval;
};

}
