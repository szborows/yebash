#pragma once

#include "History.hpp"

namespace yb {

struct HistorySuggestion {

    HistorySuggestion(const History &history) : history_(history) { position_ = history_.begin(); }

    StringOpt findSuggestion(const std::string &pattern);
    StringOpt findNextSuggestion(const std::string &pattern);

    auto &get() { return *position_; }

private:
    StringOpt find(const std::string &pattern, History::const_iterator start);
    const History &history_;
    History::const_iterator position_;
};

} // namespace yb

