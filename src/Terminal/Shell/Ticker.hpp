#pragma once

#include <chrono>
#include <functional>
#include <thread>

namespace sloptour::terminal {

class Ticker {
public:
    Ticker(std::chrono::milliseconds interval, std::function<void()> tick);

private:
    std::jthread m_thread;
};

}
