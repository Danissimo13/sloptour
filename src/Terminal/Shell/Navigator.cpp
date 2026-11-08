#include "Terminal/Shell/Navigator.hpp"

#include "Terminal/Generation/GenerationScreen.hpp"
#include "Terminal/Home/HomeScreen.hpp"
#include "Terminal/Player/PlayerScreen.hpp"
#include "Terminal/Wizard/WizardScreen.hpp"

namespace sloptour::terminal {

Destination Navigator::operator()(const GoHome& destination) const {
    return HomeScreen(m_context, destination.notice).run();
}

Destination Navigator::operator()(const GoWizard& destination) const {
    return WizardScreen(m_context, destination.kind).run();
}

Destination Navigator::operator()(const GoGenerate& destination) const {
    return GenerationScreen(m_context, destination.request).run();
}

Destination Navigator::operator()(const GoPlay& destination) const {
    auto session = m_context->services.library->open(destination.tour);
    if (!session) {
        return GoHome{.notice = session.error().message()};
    }
    return PlayerScreen(m_context, std::move(*session)).run();
}

Destination Navigator::operator()(const GoExit& /*destination*/) const {
    return GoExit{};
}

}
