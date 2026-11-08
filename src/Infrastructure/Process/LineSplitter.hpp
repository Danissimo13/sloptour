#pragma once

#include "Infrastructure/Process/LineListener.hpp"

#include <string>
#include <string_view>

namespace sloptour::infrastructure {

class LineSplitter {
public:
    void feed(std::string_view chunk, const LineListener& listener);
    void flush(const LineListener& listener);

private:
    std::string m_pending;
};

}
