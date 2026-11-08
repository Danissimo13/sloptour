#include "Infrastructure/Process/ProcessRunner.hpp"

#include "Infrastructure/Process/ExecutableLocator.hpp"
#include "Infrastructure/Process/LineSplitter.hpp"

#include <reproc++/reproc.hpp>

#include <array>
#include <cstdint>
#include <span>
#include <system_error>
#include <utility>
#include <vector>

namespace sloptour::infrastructure {

namespace {

constexpr reproc::milliseconds TERMINATE_GRACE{3000};
constexpr reproc::milliseconds KILL_GRACE{2000};
constexpr std::size_t READ_CHUNK = 8192;

[[nodiscard]] reproc::stop_actions stop_actions() {
    return {.first = {.action = reproc::stop::terminate, .timeout = TERMINATE_GRACE},
            .second = {.action = reproc::stop::kill, .timeout = KILL_GRACE},
            .third = {}};
}

class RunningProcess {
public:
    RunningProcess(ProcessListeners listeners, std::chrono::milliseconds poll_interval)
        : m_listeners(std::move(listeners))
        , m_poll_interval(std::chrono::duration_cast<reproc::milliseconds>(poll_interval)) {}

    [[nodiscard]] ProcessResult<void> start(const ProcessCommand& command) {
        reproc::options options;
        const auto directory = command.working_directory.string();
        options.working_directory = directory.c_str();
        options.stop = stop_actions();
        options.redirect.in.type = reproc::redirect::pipe;
        options.redirect.out.type = reproc::redirect::pipe;
        options.redirect.err.type = reproc::redirect::pipe;
        if (const auto error = m_process.start(launchable(command.arguments), options)) {
            return std::unexpected(
                ProcessError("cannot start '" + command.arguments.front() + "': " + error.message()));
        }
        return send(command.input);
    }

    [[nodiscard]] ProcessResult<ProcessOutcome> collect(const ProcessCommand& command, const std::stop_token& stop) {
        const auto deadline = std::chrono::steady_clock::now() + command.timeout;
        while (true) {
            const auto [events, error] = m_process.poll(reproc::event::out | reproc::event::err, m_poll_interval);
            if (error == std::errc::broken_pipe) {
                break;
            }
            if (error && error != std::errc::timed_out) {
                return std::unexpected(ProcessError("lost contact with process: " + error.message()));
            }
            if (stop.stop_requested()) {
                return halted("cancelled");
            }
            if (std::chrono::steady_clock::now() > deadline) {
                return halted("timed out");
            }
            drain(events);
        }
        return finished();
    }

private:
    [[nodiscard]] ProcessResult<void> send(std::string_view input) {
        const std::vector<std::uint8_t> bytes(input.begin(), input.end());
        auto remaining = std::span(bytes);
        while (!remaining.empty()) {
            const auto [written, error] = m_process.write(remaining.data(), remaining.size());
            if (error) {
                return std::unexpected(ProcessError("cannot send input: " + error.message()));
            }
            remaining = remaining.subspan(written);
        }
        if (const auto error = m_process.close(reproc::stream::in)) {
            return std::unexpected(ProcessError("cannot close input: " + error.message()));
        }
        return {};
    }

    void drain(int events) {
        if ((static_cast<unsigned>(events) & static_cast<unsigned>(reproc::event::out)) != 0U) {
            read(reproc::stream::out, m_outcome.output, m_output_lines, m_listeners.output);
        }
        if ((static_cast<unsigned>(events) & static_cast<unsigned>(reproc::event::err)) != 0U) {
            read(reproc::stream::err, m_outcome.errors, m_error_lines, m_listeners.errors);
        }
    }

    void read(reproc::stream stream, std::string& sink, LineSplitter& lines, const LineListener& listener) {
        const auto [size, error] = m_process.read(stream, m_buffer.data(), m_buffer.size());
        if (error) {
            return;
        }
        const auto bytes = std::span(m_buffer).first(size);
        const std::string chunk(bytes.begin(), bytes.end());
        sink += chunk;
        lines.feed(chunk, listener);
    }

    [[nodiscard]] ProcessResult<ProcessOutcome> halted(std::string_view reason) {
        m_process.stop(stop_actions());
        return std::unexpected(ProcessError(std::string(reason)));
    }

    [[nodiscard]] ProcessResult<ProcessOutcome> finished() {
        m_output_lines.flush(m_listeners.output);
        m_error_lines.flush(m_listeners.errors);
        const auto [status, error] = m_process.wait(reproc::infinite);
        if (error) {
            return std::unexpected(ProcessError("cannot wait for process: " + error.message()));
        }
        m_outcome.status = status;
        return std::move(m_outcome);
    }

    ProcessListeners m_listeners;
    reproc::milliseconds m_poll_interval;
    reproc::process m_process;
    ProcessOutcome m_outcome;
    LineSplitter m_output_lines;
    LineSplitter m_error_lines;
    std::array<std::uint8_t, READ_CHUNK> m_buffer{};
};

}

ProcessResult<ProcessOutcome> ProcessRunner::run(const ProcessCommand& command, const ProcessListeners& listeners,
                                                 const std::stop_token& stop) const {
    if (command.arguments.empty()) {
        return std::unexpected(ProcessError("empty command"));
    }
    RunningProcess process(listeners, m_poll_interval);
    if (auto started = process.start(command); !started) {
        return std::unexpected(started.error());
    }
    return process.collect(command, stop);
}

}
