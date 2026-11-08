#pragma once

#include <string>

namespace sloptour::application {

struct CommitSummary {
    std::string hash;
    std::string subject;
    std::string author;
    std::string date;
};

}
