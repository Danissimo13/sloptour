#include "Infrastructure/Process/LineSplitter.hpp"

#include "Core/Text/Text.hpp"

namespace sloptour::infrastructure {

void LineSplitter::feed(std::string_view chunk, const LineListener& listener) {
    m_pending += chunk;
    std::size_t start = 0;
    for (auto end = m_pending.find('\n'); end != std::string::npos; end = m_pending.find('\n', start)) {
        const auto line = core::trim(std::string_view(m_pending).substr(start, end - start));
        if (!line.empty()) {
            listener(line);
        }
        start = end + 1;
    }
    m_pending.erase(0, start);
}

void LineSplitter::flush(const LineListener& listener) {
    const auto line = core::trim(m_pending);
    if (!line.empty()) {
        listener(line);
    }
    m_pending.clear();
}

}
