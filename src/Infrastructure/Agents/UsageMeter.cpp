#include "Infrastructure/Agents/UsageMeter.hpp"

namespace sloptour::infrastructure {

application::TokenUsage UsageMeter::record(const UsageSample& sample) {
    if (sample.authoritative) {
        m_latest.clear();
    }
    m_latest.insert_or_assign(sample.key, sample.usage);
    application::TokenUsage total;
    for (const auto& [key, usage] : m_latest) {
        total += usage;
    }
    const auto spent = total.minus(m_total);
    m_total = total;
    return spent;
}

}
