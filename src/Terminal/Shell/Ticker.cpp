#include "Terminal/Shell/Ticker.hpp"

#include <condition_variable>
#include <mutex>
#include <utility>

namespace sloptour::terminal {

Ticker::Ticker(std::chrono::milliseconds interval, std::function<void()> tick)
    : m_thread([interval, tick = std::move(tick)](const std::stop_token& stop) {
        std::mutex mutex;
        std::condition_variable_any wake;
        std::unique_lock lock(mutex);
        while (!stop.stop_requested()) {
            if (!wake.wait_for(lock, stop, interval, [] { return false; }) && !stop.stop_requested()) {
                tick();
            }
        }
    }) {}

}
