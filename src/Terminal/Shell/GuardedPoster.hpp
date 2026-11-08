#pragma once

#include "Terminal/Shell/ScreenControls.hpp"

#include <functional>
#include <memory>
#include <utility>

namespace sloptour::terminal {

class GuardedPoster {
public:
    explicit GuardedPoster(ScreenControls controls)
        : m_controls(std::move(controls)) {}

    void post(std::function<void()> task) const {
        m_controls.post(GuardedTask{.alive = m_alive, .task = std::move(task)});
        m_controls.redraw();
    }

    void redraw() const {
        m_controls.redraw();
    }

private:
    struct GuardedTask {
        std::weak_ptr<const bool> alive;
        std::function<void()> task;

        void operator()() const {
            if (alive.lock()) {
                task();
            }
        }
    };

    ScreenControls m_controls;
    std::shared_ptr<const bool> m_alive = std::make_shared<const bool>(true);
};

}
